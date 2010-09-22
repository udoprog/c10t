#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <errno.h>

#include <sstream>
#include <string>
#include <list>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/thread.hpp>

#include <png.h>

#include "config.h"

#include "threads/threadworker.h"
#include "2d/cube.h"

#include "global.h"
#include "Level.h"
#include "Image.h"
#include "blocks.h"
#include "fileutils.h"
#include "World.h"

using namespace std;

stringstream error;
const uint8_t ERROR_BYTE = 0x01;
const uint8_t RENDER_BYTE = 0x10;
const uint8_t COMP_BYTE = 0x20;
const uint8_t IMAGE_BYTE = 0x30;

int write_image(settings_t *s, const char *filename, Image *img, const char *title)
{
   int code = 0;
   FILE *fp;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   png_bytep row = NULL;
   
   fp = fopen(filename, "wb");

   if (fp == NULL) {
      code = 1;
      goto finalise;
   }
   
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      code = 1;
      goto finalise;
   }

   info_ptr = png_create_info_struct(png_ptr);

   if (info_ptr == NULL) {
      code = 1;
      goto finalise;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
      code = 1;
      goto finalise;
   }

   png_init_io(png_ptr, fp);

   png_set_IHDR(png_ptr, info_ptr, img->get_width(), img->get_height(),
         8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = (char *)"Title";
      title_text.text = (char *)title;
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);

   row = (png_bytep) malloc(4 * img->get_width() * sizeof(png_byte));

   int x, y;
   
   for (y=0 ; y<img->get_height(); y++) {
      if (s->binary) {
        uint8_t b = ((y * 0xff) / img->get_height());
        cout << IMAGE_BYTE << b << flush;
      }
      
      for (x=0 ; x<img->get_width(); x++) {
        Color c;
        img->get_pixel(x, y, c);
        
        if (c.a == 0x0) {
          row[0 + x*4] = 0;
          row[1 + x*4] = 0;
          row[2 + x*4] = 0;
          row[3 + x*4] = 0x00;
        }
        else {
          row[0 + x*4] = c.r;
          row[1 + x*4] = c.g;
          row[2 + x*4] = c.b;
          row[3 + x*4] = 0xff;
        }
      }

      png_write_row(png_ptr, row);
   }
  
   png_write_end(png_ptr, NULL);

finalise:
   if (fp != NULL) {
     fclose(fp);
   }
    
   if (info_ptr != NULL) {
     png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   }

   if (png_ptr != NULL) {
     png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   }

   if (row != NULL) {
     free(row);
   }

   return code;
}

/*
 * Store part of a level rendered as a small image.
 *
 * This will allow us to composite the entire image later and calculate sizes then.
 */
struct partial {
  int xPos;
  int zPos;
  ImageBuffer *image;
  bool islevel;
  bool grammar_error;
  size_t grammar_error_where;
  const char* grammar_error_why;
  string path;
};

struct render_job {
  string path;
  int xPos, zPos;
};

/*static bool compare_partials(partial first, partial second)
{
  if (first.zPos < second.zPos) {
    return true;
  }
  
  if (first.zPos > second.zPos) {
    return false;
  }
  
  return first.xPos < second.xPos;;
}*/

class partial_renderer : public threadworker<render_job, partial*> {
public:
  settings_t *s;
  
  partial_renderer(settings_t *s, int n) : threadworker<render_job, partial*>(n) {
    this->s = s;
  }
  
  partial *work(render_job job) {
    Level *level = new Level(job.path.c_str(), false);
    
    partial *p = new partial;
    p->islevel = false;
    p->grammar_error = false;
    p->path = job.path;
    
    if (level->grammar_error) {
      p->grammar_error = true;
      p->grammar_error_where = level->grammar_error_where;
      p->grammar_error_why = level->grammar_error_why;
      return p;
    }
    
    if (!level->islevel) {
      return p;
    }
    
    p->islevel = true;
    p->xPos = job.xPos;
    p->zPos = job.zPos;
    
    switch (s->mode) {
    case Top:           p->image = level->get_image(s); break;
    case Oblique:       p->image = level->get_oblique_image(s); break;
    case ObliqueAngle:  p->image = level->get_obliqueangle_image(s); break;
    }

    delete level;
    return p;
  }
};

inline void calc_image_width_height(settings_t *s, int diffx, int diffz, int &image_width, int &image_height) {
  Cube c((diffz + 1) * mc::MapZ, mc::MapY, (diffx + 1) * mc::MapX);
  
  switch (s->mode) {
  case Top:
    c.get_top_limits(image_width, image_height);
    break;
  case Oblique:
    c.get_oblique_limits(image_width, image_height);
    break;
  case ObliqueAngle:
    // yes, these are meant to be flipped
    c.get_obliqueangle_limits(image_width, image_height);
    image_width += 2;
    break;
  }
}

inline void calc_image_partial(settings_t *s, partial &p, Image *all, World &world, int image_width, int image_height) {
  int diffx = world.max_x - world.min_x;
  int diffz = world.max_z - world.min_z;
  
  Cube c(diffx, 16, diffz);
  int xoffset, yoffset;
  
  switch (s->mode) {
  case Top:
    {
      point topleft(diffz - (p.zPos - world.min_z), 16, (p.xPos - world.min_x));
      c.project_top(topleft, xoffset, yoffset);
      xoffset *= mc::MapX;
      yoffset *= mc::MapZ;
      all->composite(xoffset, yoffset, *p.image);
    }
    break;
  case Oblique:
    {
      point topleft(diffz - (p.zPos - world.min_z), 16, (p.xPos - world.min_x));
      c.project_oblique(topleft, xoffset, yoffset);
      xoffset *= mc::MapX;
      yoffset *= mc::MapZ;
      all->composite(xoffset, yoffset, *p.image);
    }
    break;
  case ObliqueAngle:
    {
      point topleft(p.xPos - world.min_x, 16, p.zPos - world.min_z);
      c.project_obliqueangle(topleft, xoffset, yoffset);
      xoffset = xoffset * mc::MapX;
      yoffset = yoffset * mc::MapZ;
      all->composite(xoffset, yoffset, *p.image);
    }
    break;
  }
}

bool do_world(settings_t *s, string world_path, string output) {
  if (world_path.empty()) {
    error << "You must specify world using '-w' to generate map";
    return false;
  }
  
  if (output.empty()) {
    error << "You must specify output file using '-o' to generate map";
    return false;
  }
  
  if (!s->nocheck)
  {
    string level_dat = path_join(world_path, "level.dat");
    
    if (!is_file(level_dat)) {
      error << "could not stat file: " << level_dat << " - " << strerror(errno);
      return false;
    }
  }
  
  if (!s->silent) {
    cout << "world:  " << world_path << " " << endl;
    cout << "output: " << output << " " << endl;
    cout << endl;
  }
  
  if (!s->silent) cout << "Performing broad phase scan of world directory... " << flush;
  World world(s, world_path);
  if (!s->silent) cout << "found " << world.levels.size() << " files!" << endl;

  if (s->debug) {
    cout << "World" << endl;
    cout << "  min_x: " << world.min_x << endl;
    cout << "  max_x: " << world.max_x << endl;
    cout << "  min_z: " << world.min_z << endl;
    cout << "  max_z: " << world.max_z << endl;
  }
  
  if (!s->silent) cout << "Reading and projecting blocks on " << s->threads << " thread(s)... " << endl;

  int image_width = 0, image_height = 0;
  
  // calculate image_width / image_height
  calc_image_width_height(s, world.max_x - world.min_x, world.max_z - world.min_z, image_width, image_height);
  
  size_t approx_memory = image_width * image_height * 4 * sizeof(uint8_t);
  
  if (!s->silent) cout << "Image will be " << image_width << "x" << image_height << " and required approx. "
       << approx_memory << " bytes of memory ... " << endl;

  Image *all;
  
  if (approx_memory > s->memory_limit) {
    all = new CachedImage(s->cache_file.c_str(), image_width, image_height, s->memory_limit / sizeof(icache));
  }
  else {
    all = new MemoryImage(image_width, image_height);
  }
  // cached image in the future
  /*if (image_width * image_height > 1000) {
    all = new CachedImage("cache.dat", image_width, image_height);
  } else {
  }*/
  
  partial_renderer renderer(s, s->threads);
  renderer.start();
  unsigned int world_size = world.levels.size();
  
  std::list<level>::iterator lvlit = world.levels.begin();
  
  unsigned int lvlq = 0;
  
  for (unsigned int i = 0; i < world_size; i++) {
    if (lvlq == 0) {
      for (; lvlq < s->threads && lvlit != world.levels.end(); lvlq++) {
        level l = *lvlit;
        
        string path = world.get_level_path(l);
        
        if (s->debug) {
          cout << "using file: " << path << endl;
        }

        render_job job;
        job.path = path;
        job.xPos = l.xPos;
        job.zPos = l.zPos;
        
        renderer.give(job);
        lvlit++;
      }
    }
    
    --lvlq;
    partial *p = renderer.get();

    if (p->grammar_error) {
      if (s->require_all) {
        error << "Parser Error: " << p->path << " at (uncompressed) byte " << p->grammar_error_where
          << " - " << p->grammar_error_why;
        
        // effectively join all worker threads and prepare for exit
        renderer.join();
        return false;
      }

      if (!s->silent) {
        cout << "Ignoring unparseable file: " << p->path << " - " << p->grammar_error_why << endl;
        continue;
      }
    }
    
    if (!p->islevel) {
      if (s->debug) {
        cout << "Rejecting file since it is not a level chunk: " << p->path << endl;
      }

      continue;
    }
    
    if (!s->silent) {
      if (i % 100 == 0 && i > 0) {
        cout << " " << setw(9) << i << flush;
        
        if (i % 1000 == 0) {
          cout << endl;
        }
      }
    }
    
    if (s->binary) {
      int8_t p = (i * 0xff) / world_size;
      cout << RENDER_BYTE << p << flush;
    }
    
    calc_image_partial(s, *p, all, world, image_width, image_height);
    delete p->image;
    delete p;
  }
  
  renderer.join();
  
  if (!s->silent) cout << setw(10) << "done!" << endl;
  
  if (!s->silent) cout << "Saving image to " << output << "... " << flush;
  
  if (write_image(s, output.c_str(), all, "Map generated by c10t") != 0) {
    error << strerror(errno);
    return false;
  }
  
  if (!s->silent) cout << "done!" << endl;
  
  return true;
}

void do_help() {
  cout << "This program was made possible because of the work and inspiration by ZomBuster and Firemark" << endl;
  cout << "Written by Udoprog et al." << endl;
  cout << endl;
  cout << "Usage: c10t [options]" << endl;
  cout << "Options:" << endl
    << "  -w, --world <world>       - use this world directory as input" << endl
    << "  -o, --output <output>     - use this file as output file for generated png" << endl
    << endl
    << "  -s, --silent              - execute silently, printing nothing except errors" << endl
    << "  -h, --help                - display this help text" << endl
    << "  -v, --version             - display version information" << endl
    << "  -D, --debug               - display debug information while executing" << endl
    << "  -l, --list-colors         - list all available colors and block types" << endl
    << endl
    << "  -t, --top <int>           - splice from the top, must be less than 128" << endl
    << "  -b, --bottom <int>        - splice from the bottom, must be greater than or" << endl
    << "  -L, --limits <int-list>   - limit render to certain area. int-list form:" << endl
    << "                              North,South,West,East, e.g." << endl
    << "                              -L 0,100,-10,20 limiting between 0 and 100 in the " << endl
    << "                              north-south direction and between -10 and 20 in " << endl
    << "                              the east-west direction. " << endl
    << "                              Note: South and West are the positive directions." << endl
    << "                              equal to 0" << endl
    << endl
    << "Filtering options:" << endl
    << "  -e, --exclude <blockid>   - exclude block-id from render (multiple occurences" << endl
    << "                              is possible)" << endl
    << "  -i, --include <blockid>   - include only this block-id in render (multiple" << endl
    << "                              occurences is possible)" << endl
    << "  -a, --hide-all            - show no blocks except those specified with '-i'" << endl
    << "  -c, --cave-mode           - Cave mode - top down until solid block found," << endl
    << "                              then render bottom outlines only" << endl
    << "  -n, --night               - Night-time rendering mode" << endl
    << endl
    << "  -N, --no-check            - do not check for <world>/level.dat" << endl
    << endl
    << "Rendering options:" << endl
    << "  -q, --oblique             - oblique rendering" << endl
    << "  -y, --oblique-angle       - oblique angle rendering" << endl
    << "  -f <degrees>              - flip the rendering 90, 180 or 270 degrees CCW" << endl
    << endl
    << "  -m, --threads <int>       - Specify the amount of threads to use, for maximum" << endl
    << "                              efficency, this should match the amount of cores" << endl
    << "                              on your machine" << endl
    << endl
    << "Other Options:" << endl
    << "  -x, --binary              - Will output progress information in a binary form," << endl
    << "                              good for integration with third party tools" << endl
    << "  --require-all             - Will force c10t to require all chunks or fail" << endl
    << "                              not ignoring bad chunks" << endl
    << "  -M, --memory-limit <MB>   - Will limit the memory usage caching operations to" << endl
    << "                              file when necessary" << endl
    << "  -C, --cache-file <file>   - Cache file to use when memory usage is reached" << endl
    << endl;
  cout << endl;
  cout << "Typical usage:" << endl;
  cout << "   c10t -w /path/to/world -o /path/to/png.png" << endl;
  cout << endl;
}

int do_version() {
  cout << "c10t - a cartography tool for minecraft" << endl;
  cout << "version " << C10T_VERSION << endl;
  cout << "by: " << C10T_CONTACT << endl;
  cout << "site: " << C10T_SITE << endl;
  return 0;
}

int do_colors() {
  cout << "List of material Colors (total: " << mc::MaterialCount << ")" << endl;
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    cout << i << ": " << mc::MaterialName[i] << " = " << mc::MaterialColor[i] << endl;
  }
  
  return 0;
}

static struct option long_options[] =
 {
   {"world",            required_argument, 0, 'w'},
   {"output",           required_argument, 0, 'o'},
   {"help",             no_argument, 0, 'h'},
   {"silent",           no_argument, 0, 's'},
   {"version",          no_argument, 0, 'v'},
   {"debug",            no_argument, 0, 'D'},
   {"list-colors",      no_argument, 0, 'l'},
   {"top",              required_argument, 0, 't'},
   {"bottom",           required_argument, 0, 'b'},
   {"limits",           required_argument, 0, 'L'},
   {"memory-limit",     required_argument, 0, 'M'},
   {"cache-file",       required_argument, 0, 'C'},
   {"exclude",          required_argument, 0, 'e'},
   {"include",          required_argument, 0, 'i'},
   {"hide-all",         no_argument, 0, 'a'},
   {"no-check",         no_argument, 0, 'N'},
   {"oblique",          no_argument, 0, 'q'},
   {"oblique-angle",    no_argument, 0, 'y'},
   {"rotate",           no_argument, 0, 'r'},
   {"threads",          no_argument, 0, 'm'},
   {"cave-mode",        no_argument, 0, 'c'},
   {"require-all",      no_argument, 0, 'Q'},
   {"night",            no_argument, 0, 'n'},
   {"binary",           no_argument, 0, 'x'},
   {0, 0, 0, 0}
 };

settings_t *init_settings() {
  settings_t *s = new settings_t;
  s->excludes = new bool[mc::MaterialCount];
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    s->excludes[i] = false;
  }
  
  s->cavemode = false;
  s->excludes[mc::Air] = true;
  s->top = 127;
  s->bottom = 0;
  s->mode = Top;
  s->nocheck = false;
  s->silent = false;
  s->rotation = 0;
  s->threads = boost::thread::hardware_concurrency();
  s->binary = false;
  s->night = false;
  s->debug = false;
  s->require_all = false;
  s->use_limits = false;
  s->limits = new int[4];
  s->cache_file = "cache.dat";
  s->memory_limit = 1024 * 1024 * 1000;
  
  return s;
}

int get_blockid(const char *blockid) {
  for (int i = 0; i < mc::MaterialCount; i++) {
    if (strcmp(mc::MaterialName[i], blockid) == 0) {
      return i;
    }
  }
  
  return atoi(blockid);
}

// Convert a string such as "-30,40,50,30" to the corresponding integer array,
// and place the result in limits_rect.
void parse_limits(const char *limits_str, int*& limits_rect) {
  istringstream iss(limits_str);
  string item;
  
  for (int i=0; i < 4; i++) {
    if (!getline(iss, item, ','))
      break;
    limits_rect[i] = atoi(item.c_str());
    // negative sign added -- south is +x, west is +z.
    // also swap south and north, east and west with mod 2 stuff.
  }
}

int main(int argc, char *argv[]){
  mc::initialize_constants();

  settings_t *s = init_settings();
  
  string world;
  string output("out.png");
  int c, blockid;

  int option_index;
  
  while ((c = getopt_long(argc, argv, "DNvxcnqyalshM:C:L:w:o:e:t:b:i:m:r:", long_options, &option_index)) != -1)
  {
    blockid = -1;
    
    switch (c)
    {
    case 'e':
      blockid = get_blockid(optarg);
      assert(blockid >= 0 && blockid < mc::MaterialCount);
      s->excludes[blockid] = true;
      break;
    case 'm':
      s->threads = atoi(optarg);
      assert(s->threads > 0);
      break;
    case 'q':
      s->mode = Oblique;
      break;
    case 'v':
      return do_version();
    case 'D':
      s->debug = true;
      break;
    case 'y':
      s->mode = ObliqueAngle;
      break;
    case 'a':
      for (int i = 0; i < mc::MaterialCount; i++) {
        s->excludes[i] = true;
      }
      break;
    case 'i':
      blockid = get_blockid(optarg);
      assert(blockid >= 0 && blockid < mc::MaterialCount);
      s->excludes[blockid] = false;
      break;
    case 'w': world = optarg; break;
    case 'o': output = optarg; break;
    case 's': s->silent = true; break;
    case 'x':
      s->silent = true;
      s->binary = true;
      break;
    case 'r':
      s->rotation = atoi(optarg);
      assert(s->rotation == 90 || s->rotation == 180 || s->rotation == 270);
      break;
    case 'N': s->nocheck = true; break;
    case 'n': s->night = true; break;
    case 'c': s->cavemode = true; break;
    case 't':
      s->top = atoi(optarg);
      assert(s->top > s->bottom && s->top < mc::MapY);
      break;
    case 'L':
      parse_limits(optarg, s->limits);
      s->use_limits = true;
      break;
    case 'b':
      s->bottom = atoi(optarg);
      assert(s->bottom < s->top && s->bottom >= 0);
      break;
    case 'l':
      return do_colors();
      break;
    case 'h':
      do_help();
      return 0;
    case 'Q':
      s->require_all = true;
      break;
    case 'M':
      {
        int memory = atoi(optarg);
        assert(memory > 0);
        s->memory_limit = memory * 1024 * 1024;
      }
      break;
    case 'C':
      s->cache_file = optarg;
      break;
    case '?':
      if (optopt == 'c')
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf (stderr,
              "Unknown option character `\\x%x'.\n",
              optopt);
       return 1;
     default:
       abort ();
     }
  }
  
  if (!s->silent) {
    cout << "type '-h' for help" << endl;
    cout << endl;
  }

  if (!do_world(s, world, output))  {
    if (s->binary) {
      cout << ERROR_BYTE << flush;
      cout << error.str() << flush;
    }
    
    if (!s->silent) {
      cout << error.str() << endl;
    }

    return 1;
  }
  
  return 0;
}

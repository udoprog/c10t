#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <png.h>

#include "global.h"
#include "Level.h"
#include "Image.h"
#include "blocks.h"

using namespace std;

#ifdef _WIN32
const char *dir_sep = "\\";
const char dir_sep_c = '\\';
#else
const char *dir_sep = "/";
const char dir_sep_c = '/';
#endif

inline bool is_dir(string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   return S_ISDIR(st.st_mode);
}

inline bool is_file(string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   return S_ISREG(st.st_mode);
}

inline string path_join(string a, string b) {
  return a + dir_sep + b;
}

class dirlist {
private:
  queue<string> directories;
  queue<string> files;

public:
  dirlist(string path) {
    directories.push(path);
  }
  
  bool hasnext() {
    if (!files.empty()) {
      return true;
    }

    if (directories.empty()) {
      return false;
    }
    
    // work until you find any files
    while (!directories.empty()) {
      string path = directories.front();
      directories.pop();
      
      DIR *dir = opendir(path.c_str()); 
      
      if (!dir) {
        return false;
      }
      
      dirent *ent; 
      
      while((ent = readdir(dir)) != NULL)
      {
        string temp_str = ent->d_name;

        if (temp_str.compare(".") == 0) {
          continue;
        }
        
        if (temp_str.compare("..") == 0) {
          continue;
        }
        
        string fullpath = path_join(path, temp_str);
        
        if (is_dir(fullpath)) {
          directories.push(fullpath);
        }
        else if (is_file(fullpath)) {
          files.push(fullpath);
        }
      }
      
      closedir(dir);
    }
    
    return !files.empty();
  }

  string next() {
    string next = files.front();
    files.pop();
    return next;
  }
};

int write_image(settings_t *s, const char *filename, Image &img, const char *title)
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

   png_set_IHDR(png_ptr, info_ptr, img.get_width(), img.get_height(),
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

   row = (png_bytep) malloc(4 * img.get_width() * sizeof(png_byte));

   int x, y;
   
   for (y=0 ; y<img.get_height(); y++) {
      for (x=0 ; x<img.get_width(); x++) {
        Color c;
        img.get_pixel(x, y, c);
        
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
  Image *image;
};

bool compare_partials(partial first, partial second)
{
  if (first.zPos < second.zPos) {
    return true;
  }
  
  if (first.zPos > second.zPos) {
    return false;
  }
  
  return first.xPos < second.xPos;;
}

int do_world(settings_t *s, string world, string output) {

  if (!s->nocheck)
  {
    string level_dat = path_join(world, "level.dat");
  
    if (!is_file(level_dat)) {
      cerr << "could not stat file: " << level_dat << " - " << strerror(errno) << endl;
      return 1;
    }
  }
  
  if (!s->silent) {
    cout << "world:  " << world << " " << endl;
    cout << "output: " << output << " " << endl;
    cout << endl;
  }
  
  list<partial> partials;
  
  if (!s->silent) cout << "Reading and projecting blocks ... " << endl;
  
  dirlist listing(world);

  int i = 1;

  int minx = INT_MAX;
  int minz = INT_MAX;
  int maxx = INT_MIN;
  int maxz = INT_MIN;
  
  while (listing.hasnext()) {
    string path = listing.next();
    Level level(path.c_str());
    
    partial p;

    switch (s->mode) {
    case Top:
      p.image = level.get_image(s);
      break;
    case Oblique:
      p.image = level.get_oblique_image(s);
      break;
    case ObliqueAngle:
      p.image = level.get_obliqueangle_image(s);
      break;
    }
    
    p.xPos = level.xPos;
    p.zPos = level.zPos;
    
    if (s->flip) {
      int t = p.zPos;
      p.zPos = p.xPos;
      p.xPos = -t;
    }
    
    if (s->invert) {
      p.zPos = -p.zPos;
      p.xPos = -p.xPos;
    }
    
    if (p.xPos < minx) {
      minx = p.xPos;
    }

    if (p.xPos > maxx) {
      maxx = p.xPos;
    }

    if (p.zPos < minz) {
      minz = p.zPos;
    }
    
    if (p.zPos > maxz) {
      maxz = p.zPos;
    }

    partials.push_back(p);
    
    if (!s->silent) {
      if (i % 100 == 0) {
        cout << " " << setw(9) << i << flush;
      }
        
      if (i % 1000 == 0) {
        cout << endl;
      }
        
      ++i;
    }
  }
  
  if (!s->silent) cout << setw(10) << i << setw(10) << "done!" << endl;
  if (!s->silent) cout << "Compositioning image... " << flush;

  Image *all;
  
  switch (s->mode) {
  case Top:
    {
      int diffx = maxx - minx;
      int diffz = maxz - minz;
      int image_width = diffx * mc::MapX + mc::MapX;
      int image_height = diffz * mc::MapY + mc::MapY;
      size_t approx_memory = image_width * image_height * sizeof(nbt::Byte) * 4 * 2;
      
      if (!s->silent) cout << "png will be " << image_width << "x" << image_height << " and required approx. "
           << approx_memory << " bytes of memory ... " << flush;
      
      all = new Image(image_width, image_height);
      
      while (!partials.empty()) {
        partial p = partials.back();
        partials.pop_back();
        int xoffset = (p.xPos - minx) * mc::MapX;
        int yoffset = (p.zPos - minz) * mc::MapY;
        all->composite(xoffset, yoffset, *p.image);
        delete p.image;
      }
    }
    break;
  case Oblique:
    {
      // we must order all the partials in order to do this, otherwise compositioning might end up weird
      partials.sort(compare_partials);
      
      int diffx = maxx - minx;
      int diffz = maxz - minz;
      int image_width = diffx * mc::MapX + mc::MapX;
      int image_height = diffz * mc::MapY + mc::MapY + mc::MapZ;
      size_t approx_memory = image_width * image_height * sizeof(nbt::Byte) * 4 * 2;
      
      if (!s->silent) cout << "png will be " << image_width << "x" << image_height << " and required approx. "
           << approx_memory << " bytes of memory ... " << flush;
      
      all = new Image(image_width, image_height);
      
      while (!partials.empty()) {
        partial p = partials.front();
        partials.pop_front();
        int xoffset = (p.xPos - minx) * mc::MapX;
        int yoffset = (p.zPos - minz) * mc::MapY;
        all->composite(xoffset, yoffset, *p.image);
        delete p.image;
      }
    }
    break;
  case ObliqueAngle:
    {
      // we must order all the partials in order to do this, otherwise compositioning might end up weird
      partials.sort(compare_partials);
      
      int diffx = maxx - minx;
      int diffz = maxz - minz;
      int image_width = (diffx + diffz) * mc::MapX + 1;
      int image_height = (diffx + diffz) * mc::MapX + mc::MapZ + mc::MapX + mc::MapX + 2;
      size_t approx_memory = image_width * image_height * sizeof(nbt::Byte) * 4 * 2;
      
      if (!s->silent) cout << "png will be " << image_width << "x" << image_height << " and required approx. "
           << approx_memory << " bytes of memory ... " << flush;
      
      all = new Image(image_width, image_height);
      
      while (!partials.empty()) {
        partial p = partials.front();
        partials.pop_front();

        int mapx = (p.xPos - minx);
        int mapy = (p.zPos - minz);
        
        int xoffset = mc::MapX * mapx + mc::MapY * diffz - (mapy * mc::MapY);
        int yoffset = mc::MapX * (p.xPos - minx) + mapy * mc::MapY;
        all->composite(xoffset, yoffset, *p.image);
        delete p.image;
      }
    }
    break;
  }
  
  if (!s->silent) cout << "done!" << endl;
  if (!s->silent) cout << "Saving image to " << output << "... " << flush;
  
  if (write_image(s, output.c_str(), *all, "Map generated by c10t") != 0) {
    cerr << "failed! " << strerror(errno) << endl;
    return 1;
  }
  
  if (!s->silent) cout << "done!" << endl;

  delete all;
  return 0;
}

void do_help() {
  cout << "This program was made possible by the inspiration and work of ZomBuster and Firemark" << endl;
  cout << "Written by Udoprog" << endl;
  cout << endl;
  cout << "Usage: c10t [options]" << endl;
  cout << "Options:" << endl
    << "  -w <world-directory> - use this world directory as input" << endl
    << "  -o <outputfile>      - use this file as output file for generated png" << endl
    << endl
    << "  -s                   - execute silently, printing nothing except errors" << endl
    << "  -l                   - list all available colors and block types" << endl
    << "  -e <blockid>         - exclude block-id from render (multiple occurences is possible)" << endl
    << "  -i <blockid>         - include only this block-id in render (multiple occurences is possible)" << endl
    << "  -t <int>             - splice from the top, must be less than 128" << endl
    << "  -b <int>             - splice from the bottom, must be greater than or equal to 0" << endl
    << "  -a                   - show no blocks except those specified with '-i'" << endl
    << "  -n                   - do not check for <world>/level.dat" << endl
    << "Rendering modes:" << endl
    << "  -q                   - do oblique rendering" << endl
    << "  -y                   - do oblique angle rendering" << endl
    << "  -f                   - flip the oblique rendering 90 degrees" << endl
    << "  -r                   - flip the oblique rendering 180 degrees" << endl;
  cout << endl;
  cout << "Typical usage:" << endl;
  cout << "   c10t -w /path/to/world -o /path/to/png.png" << endl;
  cout << endl;
}

int do_colors() {
  cout << "List of material Colors (total: " << mc::MaterialCount << ")" << endl;
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    cout << i << ": " << mc::MaterialName[i] << " = " << mc::MaterialColor[i]->to_s() << endl;
  }
  
  return 0;
}

settings_t *init_settings() {
  settings_t *s = new settings_t;
  s->excludes = new bool[mc::MaterialCount];
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    s->excludes[i] = false;
  }
  
  s->excludes[mc::Air] = true;
  s->top = 127;
  s->bottom = 0;
  s->mode = Top;
  s->nocheck = false;
  s->silent = false;
  s->flip = false;
  s->invert = false;
  
  return s;
}

int get_blockid(const char *blockid) {
  for (int i = 0; i < mc::MaterialCount; i++) {
    cout << blockid << endl;
    if (strcmp(mc::MaterialName[i], blockid) == 0) {
      return i;
    }
  }
  
  return atoi(blockid);
}

int main(int argc, char *argv[]){
  mc::initialize_constants();

  settings_t *s = init_settings();
  
  string world;
  string output;
  int c, blockid;
  
  while ((c = getopt (argc, argv, "rfnqyalshw:o:e:t:b:i:")) != -1)
  {
    blockid = -1;
    
    switch (c)
    {
    case 'e':
      blockid = get_blockid(optarg);
      assert(blockid >= 0 && blockid < mc::MaterialCount);
      s->excludes[blockid] = true;
      break;
    case 'q':
      s->mode = Oblique;
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
    case 'f': s->flip = true; break;
    case 'r': s->invert = true; break;
    case 'n': s->nocheck = true; break;
    case 't':
      s->top = atoi(optarg);
      assert(s->top > s->bottom && s->top < mc::MapZ);
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
  
  if (!world.empty()) {
    if (output.empty()) {
      cout << "error: You must specify output file using '-o' to generate world" << endl;
      cout << endl;
      do_help();
      return 1;
    }
    
    return do_world(s, world, output);
  } else {
    do_help();
    return 1;
  }
  
  return 0;
};

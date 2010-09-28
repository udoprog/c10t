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

#include <boost/algorithm/string.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "config.h"

#include "threads/threadworker.h"
#include "2d/cube.h"

#include "global.h"
#include "level.h"
#include "image.h"
#include "blocks.h"
#include "fileutils.h"
#include "world.h"
#include "players.h"

using namespace std;
namespace fs = boost::filesystem;

stringstream error;
const uint8_t ERROR_BYTE = 0x01;
const uint8_t RENDER_BYTE = 0x10;
const uint8_t COMP_BYTE = 0x20;
const uint8_t IMAGE_BYTE = 0x30;
const uint8_t PARSE_BYTE = 0x40;

void cout_progress_n(int i, int all) {
  if (i == all) {
    cout << setw(6) << "done!" << endl;
  }
  else {
    if (i % 50 == 0 && i > 0) {
      cout << "." << flush;
      
      if (i % 1000 == 0) {
        cout << setw(8) << i << " " << (i * 100) / all << "%" << endl;
      }
    }
  } 
}

inline void cout_progress_b(const uint8_t type, int part, int whole) {
  uint8_t b = ((part * 0xff) / whole);
  cout << hex << std::setw(2) << setfill('0') << static_cast<int>(type)
       << hex << std::setw(2) << setfill('0') << static_cast<int>(b) << flush;
}

void cout_progress_b_render(int i, int all) {
  cout_progress_b(RENDER_BYTE, i, all);
}

void cout_progress_b_image(int i, int all) {
  cout_progress_b(IMAGE_BYTE, i, all);
}

inline void cout_error(const string& message) {
  cout << hex << std::setw(2) << setfill('0') << static_cast<int>(ERROR_BYTE)
       << hex << message << flush;
}

/*
 * Store part of a level rendered as a small image.
 *
 * This will allow us to composite the entire image later and calculate sizes then.
 */
struct Partial {
  int xPos;
  int zPos;
  image_buffer *image;
  bool islevel;
  bool grammar_error;
  size_t grammar_error_where;
  string grammar_error_why;
  string path;
};

struct render_job {
  string path;
  int xPos, zPos;
};


class PartialRenderer : public threadworker<render_job, Partial*> {
public:
  settings_t& s;
  
  PartialRenderer(settings_t& s, int n) : threadworker<render_job, Partial*>(n), s(s) {
  }
  
  Partial *work(render_job job) {
    level_file *level = new level_file(job.path.c_str());
    
    Partial *p = new Partial;
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
    
    switch (s.mode) {
    case Top:           p->image = level->get_image(s); break;
    case Oblique:       p->image = level->get_oblique_image(s); break;
    case Isometric:     p->image = level->get_isometric_image(s); break;
    case ObliqueAngle:  p->image = level->get_obliqueangle_image(s); break;
    }

    delete level;
    return p;
  }
};

inline void calc_image_width_height(settings_t& s, world_info& world, int &image_width, int &image_height) {
  int diffx = world.max_x - world.min_x;
  int diffz = world.max_z - world.min_z;
  
  Cube c((diffz + 1) * mc::MapZ, mc::MapY, (diffx + 1) * mc::MapX);
  
  switch (s.mode) {
  case Top:
    c.get_top_limits(image_width, image_height);
    break;
  case Oblique:
    c.get_oblique_limits(image_width, image_height);
    break;
  case Isometric:
    c.get_isometric_limits(image_width, image_height);
    break;
  case ObliqueAngle:
    // yes, these are meant to be flipped
    c.get_obliqueangle_limits(image_width, image_height);
    image_width += 200;
    break;
  }
}

inline void calc_image_partial(settings_t& s, Partial &p, image_base *all, world_info &world, int image_width, int image_height) {
  int diffx = world.max_x - world.min_x;
  int diffz = world.max_z - world.min_z;
  
  Cube c(diffx, 16, diffz);
  int xoffset, yoffset;
  
  switch (s.mode) {
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
  case Isometric:
    {
      point topleft(p.xPos - world.min_x, 16, p.zPos - world.min_z);
      c.project_isometric(topleft, xoffset, yoffset);
      xoffset = xoffset * mc::MapX;
      yoffset = yoffset * mc::MapZ;
      all->composite(xoffset, yoffset, *p.image);
    }
    break;
  }
}

inline void overlay_player(settings_t& s, image_base *all, world_info &world, player& p) {
  int diffx = (world.max_x - world.min_x) * mc::MapX;
  int diffz = (world.max_z - world.min_z) * mc::MapZ;
  int min_z = world.min_z * mc::MapZ;
  int min_x = world.min_x * mc::MapX;
  
  Cube c(diffx, mc::MapY, diffz);
  int xoffset, yoffset;
  
  transform_world_xz(p.xPos, p.zPos, s.rotation);
  
  color red(255, 0, 0, 255);

  image_buffer marker(16, 16, 1);
  
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      marker.set_pixel(x, y, 0, red);
      marker.set_pixel_depth(x, y, 1);
    }
  }
  
  switch (s.mode) {
  case Top:
    {
      point playerpos(diffz - (p.zPos - min_z) + mc::MapZ, p.yPos, (p.xPos - min_x));
      c.project_top(playerpos, xoffset, yoffset);
      all->composite(xoffset, yoffset, marker);
    }
    break;
  case Oblique:
    {
      point playerpos(diffz - (p.zPos - min_z) + mc::MapZ, p.yPos, (p.xPos - min_x));
      c.project_oblique(playerpos, xoffset, yoffset);
      all->composite(xoffset, yoffset, marker);
    }
    break;
  case ObliqueAngle:
    {
      point playerpos(p.xPos - min_x + mc::MapX, p.yPos, p.zPos - min_z);
      c.project_obliqueangle(playerpos, xoffset, yoffset);
      all->composite(xoffset, yoffset, marker);
    }
    break;
  case Isometric:
    {
      point playerpos(p.xPos - min_x + mc::MapX, p.yPos, p.zPos - min_z);
      c.project_isometric(playerpos, xoffset, yoffset);
      all->composite(xoffset, yoffset, marker);
    }
    break;
  }
}

bool do_one_world(settings_t &s, world_info& world, players_db& pdb, const string& output) {
  if (s.debug) {
    cout << "world_info" << endl;
    cout << "  min_x: " << world.min_x << endl;
    cout << "  max_x: " << world.max_x << endl;
    cout << "  min_z: " << world.min_z << endl;
    cout << "  max_z: " << world.max_z << endl;
    cout << "  levels: " << world.levels.size() << endl;
    cout << "  chunk pos: " << world.chunk_x << "x" << world.chunk_y << endl;
  }
  
  int i_w = 0, i_h = 0;
  
  void (*progress_c)(int part, int all) = NULL;
  
  if (!s.silent) {
    progress_c = cout_progress_n;
  }
  
  // calculate i_w / i_h
  calc_image_width_height(s, world, i_w, i_h);
  
  size_t mem_x = i_w * i_h * 4 * sizeof(uint8_t);
  float mem;
  
  if (mem_x > s.memory_limit) {
    mem = (float)(s.memory_limit) / 1000000.0f; 
    
    if (!s.silent) cout << output << ": "
         << i_w << "x" << i_h << " "
         << "~" << mem << " MB (cached at " << s.cache_file << ")... " << endl;
  } else {
    mem = (float)(i_w * i_h * 4 * sizeof(uint8_t)) / 1000000.0f; 
  
    if (!s.silent) cout << output << ": "
         << i_w << "x" << i_h << " "
         << "~" << mem << " MB... " << endl;
  }
  
  image_base *all;
  
  if (mem_x > s.memory_limit) {
    all = new cached_image(s.cache_file.c_str(), i_w, i_h, s.memory_limit / sizeof(icache));
  }
  else {
    all = new memory_image(i_w, i_h);
  }
  // cached image in the future
  /*if (image_width * i_h > 1000) {
    all = new cached_image("cache.dat", image_width, i_h);
  } else {
  }*/
  
  PartialRenderer renderer(s, s.threads);
  renderer.start();
  unsigned int world_size = world.levels.size();
  
  std::list<level>::iterator lvlit = world.levels.begin();
  
  unsigned int lvlq = 0;
  unsigned int i;

  if (s.binary) {
    progress_c = cout_progress_b_render;
  }
  
  for (i = 0; i < world_size; i++) {
    if (lvlq == 0) {
      for (; lvlq < s.threads && lvlit != world.levels.end(); lvlq++) {
        level l = *lvlit;
        
        fs::path path = world.get_level_path(l);
        
        if (s.debug) {
          cout << "using file: " << path << endl;
        }

        render_job job;
        job.path = path.string();
        job.xPos = l.xPos;
        job.zPos = l.zPos;
        
        renderer.give(job);
        lvlit++;
      }
    }
    
    --lvlq;
    Partial *p = renderer.get();

    if (p->grammar_error) {
      if (s.require_all) {
        error << "Parser Error: " << p->path << " at (uncompressed) byte " << p->grammar_error_where
          << " - " << p->grammar_error_why;
        
        // effectively join all worker threads and prepare for exit
        renderer.join();
        delete p;
        return false;
      }

      if (!s.silent) {
        cout << "Ignoring unparseable file: " << p->path << " - " << p->grammar_error_why << endl;
        delete p;
        continue;
      }
    }
    
    if (!p->islevel) {
      if (s.debug) {
        cout << "Rejecting file since it is not a level chunk: " << p->path << endl;
      }
      delete p;
      continue;
    }
    
    if (progress_c != NULL) progress_c(i, world_size);
    
    calc_image_partial(s, *p, all, world, i_w, i_h);
    delete p->image;
    delete p;
  }
  
  if (progress_c != NULL) progress_c(world_size, world_size);
  
  renderer.join();
  
  std::vector<player>::iterator plit = pdb.players.begin();
  
  /* initial code for projecting players
  for (; plit != pdb.players.end(); plit++) { 
    player p = *plit;

    if (p.zPos < s.min_z) continue;
    if (p.zPos > s.max_z) continue;
    if (p.xPos < s.min_x) continue;
    if (p.xPos > s.max_x) continue;
    
    overlay_player(s, all, world, p);
  }
  */
  
  if (!s.silent) cout << "Saving image..." << endl;
  
  if (s.binary) {
    progress_c = cout_progress_b_image;
  }
  
  if (!all->save_png(output.c_str(), "Map generated by c10t", progress_c)) {
    error << strerror(errno);
    return false;
  }
  
  delete all;
  return true;
}

bool do_world(settings_t& s, fs::path world_path, string output) {
  if (output.empty()) {
    error << "You must specify output file using '-o' to generate map";
    return false;
  }

  if (s.use_split) {
    try {
      boost::format(output) % 0 % 0;
    } catch (boost::io::too_many_args& e) {
      error << "The `-o' parameter must contain two number format specifiers `%d' (x and y coordinates) - example: -o out/base.%d.%d.png";
      return false;
    }
  }
  
  if (!s.nocheck)
  {
    fs::path level_dat = world_path / "level.dat";
    
    if (!fs::exists(level_dat)) {
      error << "Does not exist: " << level_dat;
      return false;
    }
  }
  
  players_db pdb(world_path / "players");
  
  if (!s.silent) cout << "Working on " << s.threads << " thread(s)... " << endl;
  
  if (!s.silent) {
    cout << "world:  " << world_path << " " << endl;
    cout << "output: " << output << " " << endl;
    cout << endl;
  }
  
  if (!s.silent) cout << "Performing broad phase scan of world directory... " << flush;
  world_info world(s, world_path);
  if (!s.silent) cout << "found " << world.levels.size() << " files!" << endl;

  if (!s.use_split) {
    return do_one_world(s, world, pdb, output);
  }
  
  world_info** worlds = world.split(s.split);

  int i = 0;
  int max_x = INT_MIN, max_y = INT_MIN;
  
  while (worlds[i] != NULL) {
    world_info* current = worlds[i++];

    stringstream ss;
    ss << boost::format(output) % current->chunk_x % current->chunk_y;
    
    if (!do_one_world(s, *current, pdb, ss.str())) {
      return false;
    }

    if (current->chunk_x > max_x) max_x = current->chunk_x;
    if (current->chunk_y > max_y) max_y = current->chunk_y;
  }

  cout << "world_info size in chunks is " << s.split * ((max_x + 1) * mc::MapX) << "x" << s.split * (max_y * mc::MapZ) << endl;
  
  delete [] worlds;
  return true;
}

int do_help() {
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
    << "                              North,South,East,West, e.g." << endl
    << "                              -L 0,100,-10,20 limiting between 0 and 100 in the " << endl
    << "                              north-south direction and between -10 and 20 in " << endl
    << "                              the east-west direction. " << endl
    << "                              Note: South and West are the positive directions." << endl
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
    << "  -r <degrees>              - rotate the rendering 90, 180 or 270 degrees CW" << endl
    << endl
    << "  -m, --threads <int>       - Specify the amount of threads to use, for maximum" << endl
    << "                              efficency, this should match the amount of cores" << endl
    << "                              on your machine" << endl
    << "  -B <set>                  - Specify the base color for a specific block id" << endl
    << "                              <set> has the format <blockid>=<8 digit hex>" << endl
    << "                              <8 digit hex> specifies the RGBA values as" << endl
    << "                              'RRGGBBAA'. This automatically sets the side color" << endl
    << "                              to a darkened variant of the base" << endl
    << "                              example: -s Grass=ff0000ff" << endl
    << "  -S <set>                  - Specify the side color for a specific block id" << endl
    << "                              this uses the same format as '-s' only the color" << endl
    << "                              is applied to the side of the block" << endl
    << "  -p, --split <chunks>      - Split the render into chunks, <output> must be a" << endl
    << "                              name containing two number format specifiers `%d'" << endl
    << "                              for `x' and `y' coordinates of the chunks" << endl
    << endl
    << "Other Options:" << endl
    << "  -x, --binary              - Will output progress information in a binary form," << endl
    << "                              good for integration with third party tools" << endl
    << "  --require-all             - Will force c10t to require all chunks or fail" << endl
    << "                              not ignoring bad chunks" << endl
    << "  -M, --memory-limit <MB>   - Will limit the memory usage caching operations to" << endl
    << "                              file when necessary" << endl
    << "  -C, --cache-file <file>   - Cache file to use when memory usage is reached" << endl
    << "  -P <file>                 - use <file> as palette" << endl
    << "  -W <file>                 - write <file> with the default colour palette" << endl
    << endl;
  cout << endl;
  cout << "Typical usage:" << endl;
  cout << "   c10t -w /path/to/world -o /path/to/png.png" << endl;
  cout << endl;
  return 0;
}

int do_version() {
  cout << "c10t - a cartography tool for minecraft" << endl;
  cout << "version " << C10T_VERSION << endl;
  cout << "by: " << C10T_CONTACT << endl;
  cout << "site: " << C10T_SITE << endl;
  return 0;
}

bool do_write_palette(settings_t& s, string& path) {
  if (!s.silent) cout << "Writing palette to " << path << endl;
  
  memory_image palette(16, 32);
  
  for (int x = 0; x < palette.get_width(); x++) {
    for (int y = 0, py = 0; y < palette.get_height(); y++, py += 2) {
      int p = x + palette.get_width() * y;
      
      if (p < mc::MaterialCount) {
        palette.set_pixel(x, py, *mc::MaterialColor[p]);
        palette.set_pixel(x, py + 1, *mc::MaterialSideColor[p]);
      }
    }
  }
  
  // errno should be set to something sensible here
  if (!palette.save_png(path.c_str(), "color palette for c10t", NULL)) {
    error<< "Failed to save palette " << path << " - " << strerror(errno);
    return false;
  }
  
  return true;
}

bool do_read_palette(settings_t& s, string& path) {
  if (!s.silent) cout << "Reading palette from " << path << endl;
  
  return true;
}

int do_colors() {
  cout << "List of material Colors (total: " << mc::MaterialCount << ")" << endl;
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    cout << i << ": " << mc::MaterialName[i] << " = " << *mc::MaterialColor[i] << endl;
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
   {"iso",              no_argument, 0, 'z'},
   {"rotate",           required_argument, 0, 'r'},
   {"threads",          required_argument, 0, 'm'},
   {"cave-mode",        no_argument, 0, 'c'},
   {"require-all",      no_argument, 0, 'Q'},
   {"night",            no_argument, 0, 'n'},
   {"binary",           no_argument, 0, 'x'},
   {"split",            required_argument, 0, 'p'},
   {0, 0, 0, 0}
 };

bool get_blockid(const char *blockid_string, int& blockid) {
  for (int i = 0; i < mc::MaterialCount; i++) {
    if (strcmp(mc::MaterialName[i], blockid_string) == 0) {
      blockid = i;
      return true;
    }
  }
  
  blockid = atoi(blockid_string);
  
  if (!(blockid >= 0 && blockid < mc::MaterialCount)) {
    error << "Not a valid blockid: " << blockid_string;
    return false;
  }
  
  return true;
}

bool parse_set(const char* set_str, int& blockid, color& c)
{
  istringstream iss(set_str);
  string key, value;
  
  assert(getline(iss, key, '='));
  assert(getline(iss, value));
  
  if (!get_blockid(key.c_str(), blockid)) {
    return false;
  }

  int cr, cg, cb, ca=0xff;
  
  if (!(sscanf(value.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4 || 
        sscanf(value.c_str(), "%d,%d,%d", &cr, &cg, &cb) == 3)) {
    error << "color sets must be of the form <red>,<green>,<blue>[,<alpha>] but was: " << value;
    return false;
  }
  
  if (!(
      cr >= 0 && cr <= 0xff &&
      cg >= 0 && cg <= 0xff &&
      cb >= 0 && cb <= 0xff &&
      ca >= 0 && ca <= 0xff)) {
    error << "color values must be between 0-255";
    return false;
  }
  
  c.r = cr;
  c.g = cg;
  c.b = cb;
  c.a = ca;

  return true;
}

bool do_base_color_set(const char *set_str) {
  int blockid;
  color c;
  
  if (!parse_set(set_str, blockid, c)) {
    return false;
  }
  
  delete mc::MaterialColor[blockid];
  delete mc::MaterialSideColor[blockid];
  
  mc::MaterialColor[blockid] = new color(c);
  mc::MaterialSideColor[blockid] = new color(mc::MaterialColor[blockid]);
  mc::MaterialSideColor[blockid]->darken(0x20);
  return true;
}

bool do_side_color_set(const char *set_str) {
  int blockid;
  color c;
  
  if (!parse_set(set_str, blockid, c)) {
    return false;
  }

  delete mc::MaterialSideColor[blockid];

  mc::MaterialSideColor[blockid] = new color(c);
  return true;
}

// Convert a string such as "-30,40,50,30" to the corresponding N,S,E,W integers,
// and fill in the min/max settings.
void parse_limits(const string& limits_str, settings_t& s) {
  std::vector<std::string> limits;
  boost::split(limits, limits_str, boost::is_any_of(","));
  if (limits.size() != 4) {
    return;
  }

  s.min_x = atoi(limits[0].c_str());
  s.max_x = atoi(limits[1].c_str());
  s.min_z = atoi(limits[2].c_str());
  s.max_z = atoi(limits[3].c_str());
}

int main(int argc, char *argv[]){
  mc::initialize_constants();

  settings_t s;
  
  string world_path;
  string output_path("out.png");
  string palette_write_path, palette_read_path;
  string limits;
  
  int c, blockid;

  int option_index;
  
  while ((c = getopt_long(argc, argv, "DNvxcnqzyalshM:C:L:w:o:e:t:b:i:m:r:W:P:B:S:p:", long_options, &option_index)) != -1)
  {
    blockid = -1;
    
    switch (c)
    {
    case 'v':
      return do_version();
    case 'h':
      return do_help();
    case 'e':
      if (!get_blockid(optarg, blockid)) goto exit_error;
      s.excludes[blockid] = true;
      break;
    case 'm':
      s.threads = atoi(optarg);
      
      if (s.threads <= 0) {
        error << "Number of worker threads must be more than 0";
        goto exit_error;
      }
      
      break;
    case 'p':
      s.split = atoi(optarg);
      s.use_split = true;
      assert(s.split > 1);
      break;
    case 'q':
      s.mode = Oblique;
      break;
    case 'z':
      s.mode = Isometric;
      break;
    case 'D':
      s.debug = true;
      break;
    case 'y':
      s.mode = ObliqueAngle;
      break;
    case 'a':
      for (int i = 0; i < mc::MaterialCount; i++) {
        s.excludes[i] = true;
      }
      break;
    case 'i':
      if (!get_blockid(optarg, blockid)) goto exit_error;
      s.excludes[blockid] = false;
      break;
    case 'w': world_path = optarg; break;
    case 'o': output_path = optarg; break;
    case 's': s.silent = true; break;
    case 'x':
      s.silent = true;
      s.binary = true;
      break;
    case 'r':
      s.rotation = atoi(optarg) % 360;
      if (s.rotation < 0) {
        s.rotation += 360;
      }
      if (s.rotation % 90 != 0) {
        error << "Rotation must be a multiple of 90 degrees";
        goto exit_error;
      }

      break;
    case 'N': s.nocheck = true; break;
    case 'n': s.night = true; break;
    case 'c': s.cavemode = true; break;
    case 't':
      s.top = atoi(optarg);
      
      if (!(s.top > s.bottom && s.top < mc::MapY)) {
        error << "Top limit must be between `<bottom limit> - " << mc::MapY << "', not " << s.top;
        goto exit_error;
      }
      
      break;
    case 'L':
      limits = optarg;
      break;
    case 'b':
      s.bottom = atoi(optarg);
      
      if (!(s.bottom < s.top && s.bottom >= 0)) {
        error << "Bottom limit must be between `0 - <top limit>', not " << s.bottom;
        goto exit_error;
      }
      
      break;
    case 'l':
      return do_colors();
    case 'Q':
      s.require_all = true;
      break;
    case 'M':
      {
        int memory = atoi(optarg);
        assert(memory >= 0);
        s.memory_limit = memory * 1024 * 1024;
      }
      break;
    case 'C':
      s.cache_file = optarg;
      break;
    case 'W': palette_write_path = optarg; break;
    case 'P': palette_read_path = optarg; break;
    case 'B':
      if (!do_base_color_set(optarg)) goto exit_error;
      break;
    case 'S':
      if (!do_side_color_set(optarg)) goto exit_error;
      break;
    case '?':
      if (optopt == 'c')
        error << "Option -" << optopt << " requires an argument";
      else if (isprint (optopt))
        error << "Unknown option `-" << optopt << "'";
      else
        error << "Unknown option character `\\x" << std::hex << static_cast<int>(optopt) << "'.";

       goto exit_error;
    default:
      abort ();
    }
  }

  parse_limits(limits, s);

  if (!s.silent) {
    cout << "Type `-h' for help" << endl;
  }
  
  if (!palette_write_path.empty()) {
    if (!do_write_palette(s, palette_write_path)) {
      goto exit_error;
    }
  }
  
  if (!palette_read_path.empty()) {
    if (!do_read_palette(s, palette_read_path)) {
      goto exit_error;
    }
  }
  
  if (!world_path.empty()) {
    if (!do_world(s, fs::path(world_path), output_path))  {
      goto exit_error;
    }
  }
  
  mc::deinitialize_constants();
  return 0;

exit_error:
  if (s.binary) {
    cout_error(error.str());
  }
  else {
    if (!s.silent) cout << argv[0] << ": " << error.str() << endl;
  }

  mc::deinitialize_constants();
  return 1;
}

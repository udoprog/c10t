// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef GUARD_H
#define GUARD_H

#if !defined(C10T_DISABLE_THREADS)
#  include <boost/thread.hpp>
#endif

#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>

#include <string>
#include <set>

#include "mc/blocks.hpp"
#include "image/color.hpp"
#include "2d/cube.hpp"
// #include "selectors.hpp"

namespace fs = boost::filesystem;

class chunk_selector;

typedef boost::shared_ptr<chunk_selector> pchunksel;


enum mode {
  Top = 0x0,
  Oblique = 0x1,
  ObliqueAngle = 0x2,
  Isometric = 0x3,
  FatIso = 0x4
};


enum action {
  None,
  Version,
  Help,
  GenerateWorld,
  GenerateStatistics,
  ListColors,
  WritePalette
};

struct point_surface{
	int x;
	int z;
	point_surface(int x,int z): x(x),z(z){
	}
	point_surface():x(0),z(0){};
};

struct settings_t {
  bool binary;
  bool cache_compress;
  bool cache_use;
  bool cavemode;
  bool debug;
  bool has_coordinate_color;
  bool has_player_color;
  bool has_sign_color;
  bool has_warp_color;
  bool heightmap;
  bool hellmode;
  bool memory_limit_default;
  bool night;
  bool nocheck;
  bool no_log;
  bool pedantic_broad_phase;
  bool require_all;
  bool show_coordinates;
  bool show_players;
  bool show_signs;
  bool show_warps;
  bool silent;
  bool strip_sign_prefix;
  bool striped_terrain;
  bool use_split;
  bool write_js;
  bool write_json;
  boost::shared_array<bool> excludes;
  color coordinate_color;
  color player_color;
  color sign_color;
  color ttf_color;
  color warp_color;
  enum mode mode;
  fs::path cache_dir;
  fs::path output_log;
  fs::path output_path;
  fs::path palette_read_path;
  fs::path palette_write_path;
  fs::path show_warps_path;
  fs::path statistics_path;
  fs::path swap_file;
  fs::path ttf_path;
  fs::path world_path;
  fs::path write_json_path;
  fs::path write_js_path;
  fs::path engine_path;
  bool engine_use;
  int bottom;
  uint64_t max_radius;
  int64_t min_x;
  int64_t max_x;
  int64_t min_z;
  int64_t max_z;
  int top;
  int ttf_size;
  size_t memory_limit;
  std::list<unsigned int> split;
  std::set<std::string> show_players_set;
  std::string cache_key;
  std::string show_signs_filter;
  // top/bottom used for slicing
  unsigned int prebuffer;
  unsigned int rotation;
  unsigned int split_base;
  unsigned int threads;

  int graph_block;

  int center_x;
  int center_z;

  enum action action;
  
  pchunksel selector;
  std::list< std::list<point_surface> > lines_to_follow;
  point_surface center;
 
  settings_t() {
    this->excludes.reset(new bool[mc::MaterialCount]);
    
    for (int i = 0; i < mc::MaterialCount; i++) {
      this->excludes[i] = false;
    }

    this->excludes[mc::Air] = true;

#   if !defined(C10T_DISABLE_THREADS)
      if ((this->threads = boost::thread::hardware_concurrency()) == 0) {
        this->threads = 1;
      }
#   else
      this->threads = 1;
#   endif
    this->prebuffer = 4;
    
    this->split_base = 0;
    this->use_split = false;
    this->cavemode = false;
    this->hellmode = false;
    this->top = 127;
    this->bottom = 0;
    this->mode = Top;
    this->nocheck = false;
    this->silent = false;
    this->show_players = false;
    this->show_coordinates = false;
    this->show_signs = false;
    this->strip_sign_prefix = false;
    this->show_warps = false;
    this->require_all = false;
    this->striped_terrain = false;
    this->rotation = 0;
    this->binary = false;
    this->night = false;
    this->heightmap = false;
    this->debug = false;
    this->swap_file = "swap.bin";
    this->memory_limit = 1000;
    this->memory_limit_default = true;
    this->min_x = -10000;
    this->max_x = 10000;
    this->min_z = -10000;
    this->max_z = 10000;
    this->max_radius = 1000;
    this->ttf_path = fs::path("font.ttf");
    this->ttf_size = 12;
    this->ttf_color = color(0, 0, 0, 0xff);
    this->sign_color = color(0, 0, 0, 0xff);
    this->player_color = color(0, 0, 0, 0xff);
    this->has_player_color = false;
    this->has_sign_color = false;
    this->has_coordinate_color = false;
    this->has_warp_color = false;
    this->coordinate_color = color(0, 0, 0, 0xff);
    this->warp_color = color(0, 0, 0, 0xff);
    this->pedantic_broad_phase = false;
    this->cache_use = false;
    this->cache_key = "";
    this->cache_dir = "cache";
    this->cache_compress = false;
    this->write_json = false;
    this->write_js = false;
    this->no_log = false;
    this->output_log = fs::system_complete(fs::path("c10t.log"));
    this->output_path = fs::system_complete(fs::path("out.png"));
    this->statistics_path = fs::system_complete(fs::path("statistics.txt"));
    this->graph_block = -1;
    this->action = None;

    this->center_x = 0;
    this->center_z = 0;

    this->engine_use = false;
  }
};

#endif

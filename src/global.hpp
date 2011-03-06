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

namespace fs = boost::filesystem;

enum mode {
  Top = 0x0,
  Oblique = 0x1,
  ObliqueAngle = 0x2,
  Isometric = 0x3
};

enum action {
  None,
  Version,
  Help,
  GenerateWorld,
  GenerateStatistics,
  ListColors
};

struct settings_t {
  bool cavemode;
  bool hellmode;
  bool night;
  bool heightmap;
  bool silent;
  bool nocheck;
  boost::shared_array<bool> excludes;
  bool binary;
  bool debug;
  bool use_split;
  std::list<unsigned int> split;
  unsigned int split_base;
  bool striped_terrain;
  std::set<std::string> show_players_set;
  bool require_all;
  bool pedantic_broad_phase;
  std::string show_signs_filter;
  bool show_coordinates;
  bool show_signs;
  bool show_players;
  bool disable_skylight;
  bool show_warps;
  fs::path show_warps_path;
  bool has_sign_color;
  bool has_player_color;
  bool has_coordinate_color;
  bool has_warp_color;
  // top/bottom used for slicing
  int top;
  int bottom;
  unsigned int threads;
  unsigned int prebuffer;
  enum mode mode;
  unsigned int rotation;
  int min_x, max_x, min_z, max_z; // limits to draw
  int max_radius;
  size_t memory_limit;
  bool memory_limit_default;
  int ttf_size;
  color ttf_color;
  color sign_color;
  color player_color;
  color warp_color;
  color coordinate_color;
  fs::path ttf_path;
  fs::path swap_file;
  std::string cache_key;
  fs::path cache_dir;
  bool cache_compress;
  bool cache_use;
  bool write_json;
  fs::path write_json_path;
  bool write_js;
  fs::path write_js_path;
  bool no_log;
  fs::path output_log;

  fs::path world_path;
  fs::path output_path;
  fs::path statistics_path;

  fs::path palette_write_path;
  fs::path palette_read_path;

  enum action action;
  
  settings_t() {
    this->excludes.reset(new bool[mc::MaterialCount]);
    
    for (int i = 0; i < mc::MaterialCount; i++) {
      this->excludes[i] = false;
    }

    this->excludes[mc::Air] = true;

#   if !defined(C10T_DISABLE_THREADS)
      this->threads = boost::thread::hardware_concurrency();
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
    this->show_warps = false;
    this->require_all = false;
    this->striped_terrain = false;
    this->rotation = 0;
    this->binary = false;
    this->night = false;
    this->heightmap = false;
    this->debug = false;
    this->swap_file = "swap.bin";
    this->memory_limit = 1024 * 1024 * 1000;
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
    this->disable_skylight = false;
    this->output_log = fs::system_complete(fs::path("c10t.log"));
    this->output_path = fs::system_complete(fs::path("out.png"));
    this->statistics_path = fs::system_complete(fs::path("statistics.txt"));

    this->action = None;
  }
};

#endif

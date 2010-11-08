// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef GUARD_H
#define GUARD_H

#if !defined(C10T_DISABLE_THREADS)
#  include <boost/thread.hpp>
#endif

#include <boost/filesystem.hpp>

#include <string>
#include <set>

#include "blocks.h"
#include "color.h"

namespace fs = boost::filesystem;

enum mode {
  Top,
  Oblique,
  ObliqueAngle,
  Isometric
};

struct settings_t {
  bool cavemode;
  bool night;
  bool heightmap;
  bool silent;
  bool nocheck;
  bool *excludes;
  bool binary;
  bool debug;
  bool use_split;
  bool striped_terrain;
  std::set<std::string> show_players_set;
  bool require_all;
  bool pedantic_broad_phase;
  std::string show_signs_filter;
  bool show_coordinates;
  bool show_signs;
  bool show_players;
  bool show_warps;
  fs::path show_warps_path;
  bool has_sign_color;
  bool has_player_color;
  bool has_coordinate_color;
  bool has_warp_color;
  int split;
  // top/bottom used for slicing
  int top;
  int bottom;
  unsigned int threads;
  unsigned int prebuffer;
  enum mode mode;
  unsigned int rotation;
  int min_x, max_x, min_z, max_z; // limits to draw
  size_t memory_limit;
  int ttf_size;
  color ttf_color;
  color sign_color;
  color player_color;
  color warp_color;
  color coordinate_color;
  std::string ttf_path;
  std::string cache_file;
  std::string cache_key;
  fs::path cache_dir;
  bool cache_compress;
  bool cache_use;
  bool write_markers;
  fs::path write_markers_path;
  bool use_pixelsplit;
  int pixelsplit;
  
  settings_t() {
    this->excludes = new bool[mc::MaterialCount];
    
    for (int i = 0; i < mc::MaterialCount; i++) {
      this->excludes[i] = false;
    }

#   if !defined(C10T_DISABLE_THREADS)
      this->threads = boost::thread::hardware_concurrency();
#   else
      this->threads = 1;
#   endif
    this->prebuffer = 4;
    
    this->use_split = false;
    this->split = 1;
    this->cavemode = false;
    this->excludes[mc::Air] = true;
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
    this->cache_file = "cache.dat";
    this->memory_limit = 1024 * 1024 * 1000;
    this->min_x = -10000;
    this->max_x = 10000;
    this->min_z = -10000;
    this->max_z = 10000;
    this->ttf_path = "font.ttf";
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
    this->write_markers = false;
    this->use_pixelsplit = false;
    this->pixelsplit = 0;
  }
  
  ~settings_t() {
    delete [] this->excludes;
  }
};

#endif

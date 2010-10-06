// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef GUARD_H
#define GUARD_H

#if !defined(C10T_DISABLE_THREADS)
#  include <boost/thread.hpp>
#endif

#include <string>

#include "blocks.h"
#include "color.h"

enum mode {
  Top,
  Oblique,
  ObliqueAngle,
  Isometric
};

struct settings_t {
  bool cavemode;
  bool night;
  bool silent;
  bool nocheck;
  bool *excludes;
  bool binary;
  bool debug;
  bool use_split;
  bool show_players;
  bool show_coordinates;
  bool require_all;
  bool pedantic_broad_phase;
  bool show_signs;
  bool has_sign_color;
  bool has_player_color;
  bool has_coordinate_color;
  int split;
  // top/bottom used for slicing
  int top;
  int bottom;
  unsigned int threads;
  enum mode mode;
  unsigned int rotation;
  int min_x, max_x, min_z, max_z; // limits to draw
  size_t memory_limit;
  int ttf_size;
  color ttf_color;
  color sign_color;
  color player_color;
  color coordinate_color;
  std::string ttf_path;
  std::string cache_file;
  std::string cache_key;
  
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
    this->require_all = false;
    this->rotation = 0;
    this->binary = false;
    this->night = false;
    this->debug = false;
    this->cache_file = "cache.dat";
    this->memory_limit = 1024 * 1024 * 1000;
    this->min_x = INT_MIN;
    this->max_x = INT_MAX;
    this->min_z = INT_MIN;
    this->max_z = INT_MAX;
    this->ttf_path = "font.ttf";
    this->ttf_size = 12;
    this->ttf_color = color(0, 0, 0, 0xff);
    this->sign_color = color(0, 0, 0, 0xff);
    this->has_sign_color = false;
    this->player_color = color(0, 0, 0, 0xff);
    this->has_player_color = false;
    this->coordinate_color = color(0, 0, 0, 0xff);
    this->has_coordinate_color = false;
    this->pedantic_broad_phase = false;
    this->show_signs = false;
    this->cache_key = "";
  }

  ~settings_t() {
    delete [] this->excludes;
  }
};

#endif

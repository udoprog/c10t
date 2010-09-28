// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef GUARD_H
#define GUARD_H

#include <boost/thread.hpp>
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
  bool require_all;
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
  std::string ttf_path;
  std::string cache_file;
  
  settings_t() {
    this->excludes = new bool[mc::MaterialCount];

    for (int i = 0; i < mc::MaterialCount; i++) {
      this->excludes[i] = false;
    }

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
    this->require_all = false;
    this->rotation = 0;
    this->threads = boost::thread::hardware_concurrency();
    this->binary = false;
    this->night = false;
    this->debug = false;
    this->cache_file = "cache.dat";
    this->memory_limit = 1024 * 1024 * 1000;
    this->min_x = INT_MIN;
    this->max_x = INT_MAX;
    this->min_z = INT_MIN;
    this->max_z = INT_MAX;
    this->ttf_path = "player-font.ttf";
    this->ttf_size = 12;
    this->ttf_color = color(0, 0, 0, 0xff);
  }

  ~settings_t() {
    delete [] this->excludes;
  }
};

#endif

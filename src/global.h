#ifndef GUARD_H
#define GUARD_H

#include <boost/thread.hpp>
#include <string>

#include "blocks.h"

enum mode {
  Top,
  Oblique,
  ObliqueAngle
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
  int split;
  // top/bottom used for slicing
  int top;
  int bottom;
  unsigned int threads;
  enum mode mode;
  unsigned int rotation;
  bool require_all;
  int min_x, max_x, min_z, max_z; // limits to draw
  std::string cache_file;
  size_t memory_limit;

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
    this->rotation = 0;
    this->threads = boost::thread::hardware_concurrency();
    this->binary = false;
    this->night = false;
    this->debug = false;
    this->require_all = false;
    this->cache_file = "cache.dat";
    this->memory_limit = 1024 * 1024 * 1000;
    this->min_x = INT_MIN;
    this->max_x = INT_MAX;
    this->min_z = INT_MIN;
    this->max_z = INT_MAX;
  }

  ~settings_t() {
    delete [] this->excludes;
  }
};

#endif

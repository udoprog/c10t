#ifndef GUARD_H
#define GUARD_H

#include "blocks.h"

enum mode {
  Top,
  Oblique,
  ObliqueAngle
};

typedef struct _settings {
  bool cavemode;
  bool night;
  bool silent;
  bool nocheck;
  bool *excludes;
  bool binary;
  // top/bottom used for slicing
  int top;
  int bottom;
  unsigned int threads;
  enum mode mode;
  unsigned int rotation;
  bool debug;
  bool require_all;
  int* limits; // north-south-west-east. (xmax, xmin, zmax, zmin)
  bool use_limits;
} settings_t;

#endif

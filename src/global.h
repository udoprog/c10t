#ifndef GUARD_H
#define GUARD_H

#include "blocks.h"

enum mode {
  Top,
  Oblique,
  ObliqueAngle
};

typedef struct _settings {
  bool silent;
  bool nocheck;
  bool *excludes;
  // top/bottom used for slicing
  int top;
  int bottom;
  enum mode mode;
  bool flip;
  bool invert;
} settings_t;

#endif

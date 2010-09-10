#ifndef GUARD_H
#define GUARD_H

#include "blocks.h"

enum mode {
  Top,
  Oblique
};

typedef struct _settings {
  bool silent;
  bool nocheck;
  bool *excludes;
  // top/bottom used for slicing
  int top;
  int bottom;
  enum mode mode;
} settings_t;

#endif

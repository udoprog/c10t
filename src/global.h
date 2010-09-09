#ifndef GUARD_H
#define GUARD_H

#include "blocks.h"

typedef struct _settings {
  bool silent;
  bool exclude;
  bool *excludes;
  // top/bottom used for slicing
  int top;
  int bottom;
} settings_t;

#endif

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>

namespace common {
  std::string b36encode(int number);
  int b36decode(const std::string number);
}

#endif /* _COMMON_H_ */

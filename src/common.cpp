// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "common.h"

#include <sstream>
#include <vector>
#include <stdlib.h>

const char *b36alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";
  
std::string common::b36encode(int number) {
  if (number == 0) {
    return std::string("0");
  }
  
  std::stringstream ss; 
  
  if (number < 0) {
    ss << "-";
    number = -number;
  }
  
  std::vector<char> v;

  while (number != 0) {
    div_t d = div(number, 36);
    v.push_back(b36alphabet[d.rem]);
    number = d.quot;
  }

  for (std::vector<char>::reverse_iterator it = v.rbegin(); it!=v.rend(); ++it) {
    ss << *it;
  }
  
  return ss.str();
}

int common::b36decode(const std::string num) {
  long int res = strtol(num.c_str(), NULL, 36);
  return res;
}

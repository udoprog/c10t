// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "mc/utils.hpp"

#include <sstream>
#include <vector>
#include <stdlib.h>
#include <errno.h>

namespace mc {
  namespace utils {
    using std::string;
    
    void split(std::vector<std::string>& v, const std::string& str, char delim)
    {
      std::stringstream ss(str);
      std::string item;
      
      while(std::getline(ss, item, delim)) {
        v.push_back(item);
      }
    }
    
    const char *b36alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";
      
    string b36encode(int number) {
      if (number == 0) {
        return string("0");
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
    
    int b36decode(const string num) {
      if (num.empty()) throw bad_cast();
      long int res = strtol(num.c_str(), NULL, 36);
      if (errno == ERANGE) throw bad_cast();
      return res;
    }

    int b10decode(const string num) {
      if (num.empty()) throw bad_cast();
      long int res = strtol(num.c_str(), NULL, 10);
      if (errno == ERANGE) throw bad_cast();
      return res;
    }
    
    fs::path level_dir(const fs::path base, int x, int z)
    {
      int modx = x % 64;
      if (modx < 0) modx += 64;
      int modz = z % 64;
      if (modz < 0) modz += 64;
      return base / b36encode(modx) / b36encode(modz);
    }
    
    fs::path level_path(const fs::path base, int x, int z, const string prefix, const string suffix)
    {
      return level_dir(base, x, z) / ((prefix + ".") + b36encode(x) + "." + b36encode(z) + ("." + suffix));
    }

    level_coord path_to_level_coord(const fs::path path) {
      if (!fs::is_regular_file(path)) {
        throw invalid_argument("not a regular file");
      }
      
      string extension = path.extension().string();
      
      std::vector<string> parts;
      split(parts, path.stem().string(), '.');
      
      if (parts.size() != 3 || extension.compare(".dat") != 0) {
        throw invalid_argument("level data file name does not match <x>.<z>.dat");
      }
      
      try {
        return level_coord(b36decode(parts.at(1)), b36decode(parts.at(2)));
      } catch(const bad_cast& e) {
        throw invalid_argument("could not decode coordinates from file name");
      }
    }

    level_coord path_to_region_coord(const fs::path path) {
      if (!fs::is_regular_file(path)) {
        throw invalid_argument("not a regular file");
      }
      
      string extension = path.extension().string();
      
      std::vector<string> parts;
      split(parts, path.stem().string(), '.');
      
      if (parts.size() != 3 || extension.compare(".mca") != 0) {
        throw invalid_argument("level data file name does not match <x>.<z>.mca");
      }
      
      try {
        return level_coord(b10decode(parts.at(1)) * 32, b10decode(parts.at(2)) * 32);
      } catch(const bad_cast& e) {
        throw invalid_argument("could not decode coordinates from file name");
      }
    }
  }
}

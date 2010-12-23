#include "mcutils.hpp"

namespace mcutils {
  using common::b36encode;
  using std::string;
  
  fs::path level_dir(const fs::path base, int x, int z)
  {
    int modx = x % 64;
    if (modx < 0) modx += 64;
    int modz = z % 64;
    if (modz < 0) modz += 64;
    return base / b36encode(modx) / b36encode(modz);
  }
  
  fs::path level_path(const fs::path base, int x, int z, const std::string prefix, const std::string suffix)
  {
    return level_dir(base, x, z) / ((prefix + ".") + b36encode(x) + "." + b36encode(z) + ("." + suffix));
  }
};

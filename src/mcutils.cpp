#include "mcutils.hpp"
#include "algorithm.hpp"

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

  void path_to_level_coord(const fs::path path, level_coord& coord) {
    if (!fs::is_regular_file(path)) {
      throw invalid_argument("is not a regular file");
    }
    
    std::string extension = fs::extension(path);
    
    std::vector<std::string> parts;
    nonstd::split(parts, fs::basename(path), '.');
    
    if (parts.size() != 3 || extension.compare(".dat") != 0) {
      throw invalid_argument("level data file name does not match <x>.<z>.dat");
    }
    
    try {
      coord.x = common::b36decode(parts.at(1));
      coord.z = common::b36decode(parts.at(2));
    } catch(const common::bad_cast& e) {
      throw invalid_argument("could not decode coordinates from file name");
    }
  }
};

#include "mc/region_iterator.hpp"
#include "mc/region.hpp"
#include "dirlist.hpp"

bool dir_filter(const std::string& name)
{
  if (name.compare(0, 3, "DIM") == 0) return false;
  if (name.compare("players") == 0) return false;
  if (name.compare("poi") == 0) return false;
  if (name.compare("entities") == 0) return false;
  return true;
}

bool file_filter(const std::string& name) {
  if (name.length() < 8) return false;
  if (name.compare(name.length() - 4, 4, ".mca") != 0) return false;
  if (name.compare(0, 2, "r.") != 0) return false;
  return true;
}

namespace mc {
  region_iterator::region_iterator(const fs::path path)
    : root(path), lister(new dirlist(path))
  {
  }

  bool region_iterator::has_next()
  {
    if (!lister->has_next(dir_filter, file_filter)) {
      return false;
    }

    fs::path next = lister->next();
    current_region.reset(new region(next));
    return true;
  }

  region_ptr region_iterator::next() {
    return current_region;
  }
}

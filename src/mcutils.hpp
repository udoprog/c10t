#ifndef __MCUTILS_HPP__
#define __MCUTILS_HPP__

#include <boost/filesystem.hpp>

#include "common.hpp"

namespace fs = boost::filesystem;

namespace mcutils {
  fs::path level_dir(const fs::path base, int x, int z);
  fs::path level_path(const fs::path base, int x, int z, const std::string prefix, const std::string suffix);
};

#endif /* __MCUTILS_HPP__ */

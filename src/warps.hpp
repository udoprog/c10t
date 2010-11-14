#ifndef _WARPS_H_
#define _WARPS_H_

#include <string>
#include <vector>
#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "global.hpp"
#include "nbt/nbt.hpp"

namespace fs = boost::filesystem;

class warp {
public:
  std::string name;

  nbt::Int xPos, yPos, zPos;
};

class warps_db {
public:
  fs::path path;
  bool fatal;
  std::string fatal_why;
  std::vector<warp> warps;
  
  warps_db(fs::path path, bool disable);
};

#endif /* _WARPS_H_ */

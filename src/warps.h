#ifndef _WARPS_H_
#define _WARPS_H_

#include <string>
#include <vector>
#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "global.h"
#include "nbt/nbt.h"

namespace fs = boost::filesystem;

class warp {
public:
  std::string name;

  nbt::Int xPos, yPos, zPos;
};

class warps_db {
public:
  std::vector<warp> warps;

  warps_db(settings_t& s);
};

#endif /* _WARPS_H_ */

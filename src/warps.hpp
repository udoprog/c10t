#ifndef _WARPS_H_
#define _WARPS_H_

#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "nbt/nbt.hpp"

namespace fs = boost::filesystem;

class warps_db_exception : public std::exception {
  private:
    const char* why;
  public:
    warps_db_exception(const char* why) : why(why) { }
    
    const char* what() throw() {
      return this->why;
    }
};

class warp {
public:
  std::string name;
  nbt::Int xPos, yPos, zPos;
};

class warps_db {
public:
  const fs::path path;
  
  warps_db(const fs::path path) : path(path) {}
  void read(std::vector<warp>&);
};

#endif /* _WARPS_H_ */

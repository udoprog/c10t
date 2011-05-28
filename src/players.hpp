// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <exception>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "global.hpp"
#include "nbt/nbt.hpp"

namespace fs = boost::filesystem;

class players_db_exception : public std::exception {
  private:
    const char* message;
  public:
    players_db_exception(const char* message) : message(message) { }
    const char* what() throw() {
      return message;
    }
};

class player {
public:
  fs::path path;
  std::string name;
  
  bool error;
  size_t error_where;
  std::string error_why;
  bool in_pos;
  int pos_c;
  nbt::Int xPos, yPos, zPos;
  
  player(const fs::path path);
};

class players_db {
  private:
    const fs::path path;
    const std::set<std::string> filter_set;
  public:
    players_db(const fs::path path, std::set<std::string> set) : path(path), filter_set(set) {}
    void read(std::vector<player>&) const;
};

#endif /* _PLAYERS_H_ */

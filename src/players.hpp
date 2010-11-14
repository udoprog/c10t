// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include <string>
#include <vector>
#include <set>
#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "global.hpp"
#include "nbt/nbt.hpp"

namespace fs = boost::filesystem;

class player {
public:
  fs::path path;
  std::string name;

  bool grammar_error;
  bool in_pos;
  int pos_c;
  nbt::Int xPos, yPos, zPos;
  
  player(const fs::path path);
};

class players_db {
public:
  fs::path path;
  bool fatal;
  std::string fatal_why;
  
  std::vector<player> players;
  
  players_db(const fs::path path, std::set<std::string>& match_set, bool disable);
};

#endif /* _PLAYERS_H_ */

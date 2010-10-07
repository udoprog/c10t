// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include <string>
#include <vector>
#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "nbt/nbt.h"

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
  std::vector<player> players;

  players_db(const fs::path path) {
    fs::path full_path = fs::system_complete( path );
    
    if (!fs::is_directory(full_path)) {
      return;
    }

    fs::directory_iterator end_iter;
    
    for ( fs::directory_iterator dir_itr( full_path );
          dir_itr != end_iter;
          ++dir_itr )
    {
      player p(dir_itr->path());
      
      if (p.grammar_error) {
        continue;
      }
      
      players.push_back(p);
    }
  }
};

#endif /* _PLAYERS_H_ */

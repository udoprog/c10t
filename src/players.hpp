// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include <string>
#include <vector>
#include <set>
#include <exception>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "settings_t.hpp"
#include "nbt/nbt.hpp"

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
  boost::filesystem::path path;
  std::string name;
  
  bool error;
  size_t error_where;
  std::string error_why;
  bool in_pos;
  int pos_c;
  nbt::Int xPos, yPos, zPos;
  
  player(const boost::filesystem::path path);
};

class players_db {
  private:
    const boost::filesystem::path path;
    const std::set<std::string> filter_set;
  public:
    players_db(const boost::filesystem::path path, std::set<std::string> set);
    void read(std::vector<player>&) const;
};

#endif /* _PLAYERS_H_ */

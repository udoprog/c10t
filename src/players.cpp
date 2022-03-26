// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.

#include <boost/regex.hpp>

#include "players.hpp"

namespace fs = boost::filesystem;

void error_handler(player *p, size_t where, const char* why) {
  p->error_where = where;
  p->error_why = why;
  p->error = true;
}

void begin_list(player *p, std::string name, nbt::Byte, nbt::Int) {
  if (name.compare("Pos") == 0) {
    p->in_pos = true;
  }
}

void end_list(player *p, std::string name) {
  if (name.compare("Pos") == 0) {
    p->in_pos = false;
  }
}

void register_double(player *p, std::string name, nbt::Double value) {
  if (p->in_pos) {
    switch (p->pos_c) {
      case 0: p->xPos = value; break;
      case 1: p->yPos = value; break;
      case 2: p->zPos = value; break;
    }

    p->pos_c++;
  }
}

player::player(const fs::path path) :
  path(path),
  name(fs::basename(path)),
  error(false), error_where(0), error_why(""), in_pos(false),
  pos_c(0), xPos(0), yPos(0), zPos(0)
{
  nbt::Parser<player> parser(this);
  parser.error_handler = error_handler;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.register_double = register_double;
  parser.parse_file(path.string().c_str());
}

players_db::players_db(fs::path path, std::set<std::string> set)
  : path(path), filter_set(set)
{
}

const boost::regex player_extension( "[^\\.]*\\.dat" );

void players_db::read(std::vector<player>& players) const
{
  fs::path full_path = fs::system_complete( path );

  if (!fs::is_directory(full_path)) {
    throw players_db_exception("database does not exist");
  }

  fs::directory_iterator end_iter;

  for (fs::directory_iterator dir_itr(full_path);
        dir_itr != end_iter;
        ++dir_itr )
  {
    // Player files are simply dat-files; filter for them
    // to avoid picking up dat_old-files or any other
    // unexpeced files.
    boost::smatch match;
    if (!boost::regex_match(dir_itr->path().filename().string(), match, player_extension)) {
      continue;
    }

    // player files have always been .dat; TODO filter that since .dat_old is used.. sometimes?
    player p(dir_itr->path());

    if (filter_set.size() > 0) {
      if (filter_set.find(p.name) == filter_set.end()) {
        continue;
      }
    }

    players.push_back(p);
  }
}

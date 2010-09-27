#include "players.h"

void error_handler(player *p, size_t where, const char* why) {
  p->grammar_error = true;
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
  name(fs::basename(path)), grammar_error(false), in_pos(false),
  pos_c(0), xPos(0), yPos(0), zPos(0)
{
  nbt::Parser<player> parser(this);
  parser.error_handler = error_handler;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.register_double = register_double;
  parser.parse_file(path.string().c_str());
}

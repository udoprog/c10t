// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include <assert.h>

#include "global.h"

#include "level.h"
#include "blocks.h"
#include "2d/cube.h"
#include "common.h"
#include "algorithm.hpp"

#include <boost/algorithm/string.hpp>
#include <vector>
#include <fstream>

#include <ctime>

void begin_compound(level_file* level, nbt::String name) {
  if (name.compare("Level") == 0) {
    level->islevel = true;
    return;
  }
}

void register_string(level_file* level, nbt::String name, nbt::String value) {
  if (!level->in_te) {
    return;
  }
  
  if (level->in_sign) {
    if (level->sign_text.size() == 0) {
      level->sign_text = value;
    }
    else {
      level->sign_text += "\n" + value;
    }
    
    return;
  }
  
  if (name.compare("id") == 0 && value.compare("Sign") == 0) {
    level->in_sign = true;
  }
}

void register_int(level_file* level, nbt::String name, nbt::Int i) {
  if (level->in_te) {
    if (level->in_sign) {
      if (name.compare("x") == 0) {
        level->sign_x = i;
      }
      else if (name.compare("y") == 0) {
        level->sign_y = i;
      }
      else if (name.compare("z") == 0) {
        level->sign_z = i;
      }
    }
    
    return;
  }
}

void register_byte_array(level_file* level, nbt::String name, nbt::ByteArray* byte_array) {
  if (!level->islevel) {
    delete byte_array;
    return;
  }
  
  if (name.compare("Blocks") == 0) {
    level->blocks.reset(byte_array);
    return;
  }
  
  if (name.compare("SkyLight") == 0) {
    level->skylight.reset(byte_array);
    return;
  }

  if (name.compare("HeightMap") == 0) {
    level->heightmap.reset(byte_array);
    return;
  }
  
  if (name.compare("BlockLight") == 0) {
    level->blocklight.reset(byte_array);
    return;
  }
  
  delete byte_array;
}

void begin_list(level_file* level, nbt::String name, nbt::Byte type, nbt::Int count) {
  if (name.compare("TileEntities") == 0) {
    level->in_te = true;
  }
}

void end_list(level_file* level, nbt::String name) {
  if (name.compare("TileEntities") == 0) {
    level->in_te = false;
  }
}

void end_compound(level_file* level, nbt::String name) {
  if (level->in_te) {
    if (level->in_sign) {
      level->in_sign = false;
      light_marker m(level->sign_text, level->sign_x, level->sign_y, level->sign_z);
      level->markers.push_back(m);
      level->sign_text = "";
      level->sign_x = 0;
      level->sign_y = 0;
      level->sign_z = 0;
    }
  }
}

void error_handler(level_file* level, size_t where, const char *why) {
  level->grammar_error = true;
  level->grammar_error_where = where;
  level->grammar_error_why = why;
}

#include <iostream>

level_file::~level_file(){
}

level_file::level_file(fs::path path)
    :
    islevel(false),
    grammar_error(false),
    grammar_error_where(0),
    grammar_error_why(""),
    in_te(false), in_sign(false),
    sign_x(0), sign_y(0), sign_z(0),
    sign_text("")
{
  nbt::Parser<level_file> parser(this);
  
  parser.register_byte_array = register_byte_array;
  parser.register_string = register_string;
  parser.register_int = register_int;
  parser.begin_compound = begin_compound;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.end_compound = end_compound;
  parser.error_handler = error_handler;
  
  parser.parse_file(path.string().c_str());
}

void fast_begin_compound(fast_level_file* level, nbt::String name) {
  if (name.compare("Level") == 0) {
    level->islevel = true;
  }
}

void fast_register_int(fast_level_file* level, nbt::String name, nbt::Int i) {
  if (!level->islevel) {
    return;
  }

  if (name.compare("xPos") == 0) {
    level->has_xPos = true;
    level->xPos = i;
  }
  else if (name.compare("zPos") == 0) {
    level->has_zPos = true;
    level->zPos = i;
  }
  
  if (level->has_xPos && level->has_zPos) {
    level->parser.stop();
  }
}

void fast_error_handler(fast_level_file* level, size_t where, const char *why) {
  level->grammar_error = true;
  level->grammar_error_where = where;
  level->grammar_error_why = why;
}

fast_level_file::fast_level_file(const fs::path path, bool force_parsing)
  :
    xPos(0), zPos(0),
    has_xPos(false), has_zPos(false),
    islevel(false),
    grammar_error(false),
    grammar_error_where(0),
    grammar_error_why(""),
    path(path),
    parser(this)
{
  std::string extension = fs::extension(path);
  
  if (force_parsing) {
    if (extension.compare(".dat") != 0) {
      grammar_error = true;
      grammar_error_why = "File extension is not .dat";
      return;
    }

    parser.register_int = fast_register_int;
    parser.begin_compound = fast_begin_compound;
    parser.error_handler = fast_error_handler;
    
    parser.parse_file(path.string().c_str());
    return;
  }

  std::vector<std::string> parts;
  nonstd::split(parts, fs::basename(path), '.');
  
  if (parts.size() != 3 || extension.compare(".dat") != 0) {
    grammar_error = true;
    grammar_error_why = "Filename does not match c.<x>.<z>.dat";
    return;
  }
  
  std::string x = parts.at(1);
  std::string z = parts.at(2);
  
  try {
    xPos = common::b36decode(x);
    zPos = common::b36decode(z);
  } catch(const common::bad_cast& e) {
    grammar_error = true;
    grammar_error_why = "Filename does not match c.<x>.<z>.dat";
    return;
  }
  
  islevel = true;
}

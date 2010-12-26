// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include <assert.h>

#include "global.hpp"

#include "level.hpp"
#include "blocks.hpp"
#include "2d/cube.hpp"
#include "common.hpp"
#include "algorithm.hpp"

#include <boost/algorithm/string.hpp>
#include <vector>
#include <fstream>

#include <ctime>

struct level_file_context {
  bool islevel;
  bool in_te, in_sign;
  nbt::Int sign_x, sign_y, sign_z;
  std::string sign_text;
  std::vector<light_marker> markers;
  
  boost::shared_ptr<nbt::ByteArray> blocks;
  boost::shared_ptr<nbt::ByteArray> skylight;
  boost::shared_ptr<nbt::ByteArray> heightmap;
  boost::shared_ptr<nbt::ByteArray> blocklight;
  
  bool grammar_error;
  size_t grammar_error_where;
  const char* grammar_error_why;

  level_file_context() : grammar_error(false), grammar_error_where(0), grammar_error_why("") {
  }
};

void begin_compound(level_file_context* level, nbt::String name) {
  if (name.compare("Level") == 0) {
    level->islevel = true;
    return;
  }
}

void register_string(level_file_context* level, nbt::String name, nbt::String value) {
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

void register_int(level_file_context* level, nbt::String name, nbt::Int i) {
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

void register_byte_array(level_file_context* level, nbt::String name, nbt::ByteArray* byte_array) {
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

void begin_list(level_file_context* level, nbt::String name, nbt::Byte type, nbt::Int count) {
  if (name.compare("TileEntities") == 0) {
    level->in_te = true;
  }
}

void end_list(level_file_context* level, nbt::String name) {
  if (name.compare("TileEntities") == 0) {
    level->in_te = false;
  }
}

void end_compound(level_file_context* level, nbt::String name) {
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

void error_handler(level_file_context* level, size_t where, const char *why) {
  level->grammar_error = true;
  level->grammar_error_where = where;
  level->grammar_error_why = why;
}

#include <iostream>

level_file::~level_file(){
}

level_file::level_file(fs::path path) : path(path) {}

void level_file::read() {
  level_file_context context;
  
  nbt::Parser<level_file_context> parser(&context);
  
  parser.register_byte_array = register_byte_array;
  parser.register_string = register_string;
  parser.register_int = register_int;
  parser.begin_compound = begin_compound;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.end_compound = end_compound;
  parser.error_handler = error_handler;
  
  parser.parse_file(path.string().c_str());
  
  if (context.grammar_error) {
    throw invalid_file("not a valid nbt file");
  }
  
  if (!context.islevel) {
    throw invalid_file("not a level data file");
  }
  
  markers = context.markers;
  blocks = context.blocks;
  skylight = context.skylight;
  heightmap = context.heightmap;
  blocklight = context.blocklight;
  
  complete = true;
}

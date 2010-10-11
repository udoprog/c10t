// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include <assert.h>

#include "global.h"

#include "level.h"
#include "blocks.h"
#include "2d/cube.h"
#include "common.h"

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
    if (value.size() == 0) {
      return;
    }
    
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
  
  if (!level->islevel) {
    return;
  }

  if (name.compare("xPos") == 0) {
    level->xPos = i;
    return;
  }
  
  if (name.compare("zPos") == 0) {
    level->zPos = i;
    return;
  }
}

void register_byte_array(level_file* level, nbt::String name, nbt::ByteArray* byte_array) {
  if (!level->islevel) {
    delete byte_array;
    return;
  }
  
  if (name.compare("Blocks") == 0) {
    level->blocks = byte_array;
    return;
  }
  
  if (name.compare("SkyLight") == 0) {
    level->skylight = byte_array;
    return;
  }

  if (name.compare("HeightMap") == 0) {
    level->heightmap = byte_array;
    return;
  }
  
  if (name.compare("BlockLight") == 0) {
    level->blocklight = byte_array;
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
  delete blocks;
  delete skylight;
  delete heightmap;
  delete blocklight;
}

level_file::level_file(settings_t& s, const fs::path path)
  : blocks(NULL),
    skylight(NULL),
    heightmap(NULL),
    blocklight(NULL),
    xPos(0),
    zPos(0),
    islevel(false),
    grammar_error(false),
    grammar_error_where(0),
    grammar_error_why(""),
    path(path),
    in_te(false), in_sign(false),
    sign_x(0), sign_y(0), sign_z(0),
    sign_text(""),
    cache(s),
    cache_use(s.cache_use)
{
  cache_hit = false;
  oper = new image_operations;
  
  if (cache_use) {
    cache.set_path(s.cache_dir / ( fs::basename(path) + ".cmap" ));
    
    std::time_t level_mod = fs::last_write_time(path);
  
    if (fs::exists(cache.get_path())) {
      if (!cache.read(oper, level_mod)) {
        fs::remove(cache.get_path());
      }
      else {
        cache_hit = true;
      }
    }
    
    if (cache_hit) {
      islevel = true;
      return;
    }
    
    // in case of future writes, save the modification time
    cache.set_modification_time(level_mod);
  }
  
  nbt::Parser<level_file> parser(this);
  
  parser.register_byte_array = register_byte_array;
  parser.register_int = register_int;
  parser.register_string = register_string;
  parser.begin_compound = begin_compound;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.begin_compound = begin_compound;
  parser.end_compound = end_compound;
  parser.error_handler = error_handler;
  
  parser.parse_file(path.string().c_str());
}

class BlockRotation {
private:
  settings_t& s;
  nbt::ByteArray *byte_array;

  void transform_xz(int& x, int& z) {
    int t = x;
    switch (s.rotation) {
      case 270:
        x = mc::MapX - z - 1;
        z = t;
        break;
      case 180:
        z = mc::MapZ - z - 1;
        x = mc::MapX - x - 1;
        break;
      case 90:
        x = z;
        z = mc::MapZ - t - 1;
        break;
    };
  }

public:
  BlockRotation(settings_t& s, nbt::ByteArray *byte_array)
    : s(s), byte_array(byte_array) {}
  
  uint8_t get8(int x, int z, int y) {
    transform_xz(x, z);
    
    int p = y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ);
    assert (p >= 0 && p < byte_array->length);
    return byte_array->values[p];
  }
  
  uint8_t get8(int x, int z) {
    transform_xz(x, z);
    int p = x + (z * mc::MapX);
    assert (p >= 0 && p < byte_array->length);
    return byte_array->values[p];
  }
  
  int get4(int x, int z, int y) {
    int p = (y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ)) >> 1;
    if (!(p >= 0 && p < byte_array->length)) return -1;
    return ((byte_array->values[p]) >> ((y % 2) * 4)) & 0xf;
  }
};

/**
 * Blocks[ z + ( y * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
inline void apply_shading(settings_t& s, int bl, int sl, int hm, int y, color &c) {
  // if night, darken all colors not emitting light
  
  if(s.night) {
    c.darken(0xa * (16 - bl));
  }
  else if (sl != -1 && y != s.top) {
    c.darken(0xa * (16 - std::max(sl, bl)));
  }
  
  //c.darken((mc::MapY - y));
  
  // in heightmap mode, brightness = height
  if (s.heightmap) {
    c.b = y*2;
    c.g = y*2;
    c.r = y*2;
    c.a = 0xff;
  }
  
  if (s.striped_terrain && y % 2 == 0) {
    c.darken(0xf);
  }
}

inline bool cavemode_isopen(int bt) {
  switch(bt) {
    case mc::Air: return true;
    case mc::Leaves: return true;
    default: return false;
  }
}

inline bool cavemode_ignore_block(settings_t& s, int x, int z, int y, int bt, BlockRotation& b_r, bool &cave_initial) {
  if (cave_initial) {
    if (!cavemode_isopen(bt)) {
      cave_initial = false;
      return true;
    }
    
    return true;
  }
  
  if (!cavemode_isopen(bt)) {
    if (y < s.top && cavemode_isopen(b_r.get8(x, z, y + 1))) {
      return false;
    }
  }

  return true;
}

image_operations* level_file::get_image(settings_t& s) {
  if (cache_hit) return oper;
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  if (!islevel) {
    return oper;
  }
  
  // block type
  BlockRotation b_r(s, blocks);
  BlockRotation bl_r(s, blocklight);
  BlockRotation sl_r(s, skylight);
  
  for (int z = mc::MapZ - 1; z >= 0; z--) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;
      
      // do incremental color fill until color is opaque
      for (int y = s.top; y > s.bottom; y--) {
        int bt = b_r.get8(x, z, y);
        
        if (s.cavemode && cavemode_ignore_block(s, x, z, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        color bc = mc::MaterialColor[bt];
        
        apply_shading(s, bl_r.get4(x, z, y), sl_r.get4(x, z, y + 1), 0, y, bc);
        
        point p(x, y, z);
        
        int px, py;

        c.project_top(p, px, py);
        
        oper->add_pixel(px, py, bc);
        
        if (bc.is_opaque()) {
          break;
        }
      }
    }
  }

  if (cache_use) {
    if (!cache.write(oper)) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
}

image_operations* level_file::get_oblique_image(settings_t& s)
{
  if (cache_hit) return oper;
  
  if (!islevel) {
    return oper;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  // block type
  int bt;
      
  BlockRotation b_r(s, blocks);
  BlockRotation bl_r(s, blocklight);
  BlockRotation sl_r(s, skylight);
  
  int bmx, bmy, bmt;
  c.get_oblique_limits(bmx, bmy);
  bmt = bmx * bmy;
  bool blocked[bmt];
  
  for (int i = 0; i < bmt; i++) { blocked[i] = false; }
  
  for (int x = c.x - 1; x >= 0; x--) {
    for (int z = 0; z < c.z; z++) {
      bool cave_initial = true;
      int cavemode_top = s.top;
      
      if (s.cavemode) {
        for (int y = s.top; y > 0; y--) {
          bt = b_r.get8(x, z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        
        bt = b_r.get8(x, z, y);
        
        if (s.cavemode) {
          if (cavemode_ignore_block(s, x, z, y, bt, b_r, cave_initial) || y >= cavemode_top) {
            continue;
          }
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = bl_r.get4(x, z, y);
        
        int px, py;
        c.project_oblique(p, px, py);
        
        color top = mc::MaterialColor[bt];

        int bx, by;
        c.project_oblique(p, bx, by);
        
        int bp = bx + bmx * by;
        
        if (blocked[bp]) {
          continue;
        }
        
        blocked[bp] = top.is_opaque();
        apply_shading(s, bl, sl_r.get4(x, z, y + 1), 0, y, top);
        oper->add_pixel(px, py, top);
        
        color side = mc::MaterialSideColor[bt];
        apply_shading(s, bl, -1, 0, y, side);
        oper->add_pixel(px, py + 1, side);
      }
    }
  }
  
  if (cache_use) {
    if (!cache.write(oper)) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
}

image_operations* level_file::get_obliqueangle_image(settings_t& s)
{
  if (cache_hit) return oper;
  
  if (!islevel) {
    return oper;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  // block type
  int bt;
      
  BlockRotation b_r(s, blocks);
  BlockRotation bl_r(s, blocklight);
  BlockRotation sl_r(s, skylight);
  BlockRotation hm_r(s, heightmap);

  int bmx, bmy, bmt;
  c.get_obliqueangle_limits(bmx, bmy);
  bmt = bmx * bmy;
  bool blocked[bmt];
  
  for (int i = 0; i < bmt; i++) { blocked[i] = false; }
  
  for (int z = c.z - 1; z >= 0; z--) {
    for (int x = c.x - 1; x >= 0; x--) {
      bool cave_initial = true;
      
      int cavemode_top = s.top;

      if (s.cavemode) {
        for (int y = s.top; y > 0; y--) {
          bt = b_r.get8(x, z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }

      int hmval = hm_r.get8(x, z);
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        
        bt = b_r.get8(x, z, y);
        
        if (s.cavemode) {
          if (cavemode_ignore_block(s, x, z, y, bt, b_r, cave_initial) || y >= cavemode_top) {
            continue;
          }
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = bl_r.get4(x, z, y);
        
        int px, py;
        c.project_obliqueangle(p, px, py);
        
        color top = mc::MaterialColor[bt];
        color side = mc::MaterialSideColor[bt];
        
        apply_shading(s, bl, sl_r.get4(x, z, y + 1), hmval, y, top);
        apply_shading(s, bl, -1, hmval, y, side);
        
        int bx, by;
        c.project_obliqueangle(p, bx, by);
        
        int bp = bx + bmx * by;
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque();
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          oper->add_pixel(px, py + 1, side);
          
          side.lighten(0x20);
          oper->add_pixel(px + 1, py + 1, side);
          break;
        case mc::HalfBlock:
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          break;
        case mc::TopBlock:
          oper->add_pixel(px, py - 1, top);
          oper->add_pixel(px + 1, py - 1, top);
          break;
        }
      }
    }
  }

  if (cache_use) {
    if (!cache.write(oper)) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
}

image_operations* level_file::get_isometric_image(settings_t& s)
{
  if (cache_hit) return oper;
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);

  int iw, ih;
  c.get_isometric_limits(iw, ih);
  
  if (!islevel) {
    return oper;
  }
  
  // block type
  int bt;
      
  BlockRotation b_r(s, blocks);
  BlockRotation bl_r(s, blocklight);
  BlockRotation sl_r(s, skylight);
  BlockRotation hm_r(s, heightmap);
  
  int bmx, bmy, bmt;
  c.get_isometric_limits(bmx, bmy);
  bmt = bmx * bmy;
  bool blocked[bmt];
  
  for (int i = 0; i < bmt; i++) { blocked[i] = false; }
  
  for (int z = c.z - 1; z >= 0; z--) {
    for (int x = c.x - 1; x >= 0; x--) {
      bool cave_initial = true;
      
      int cavemode_top = s.top;

      if (s.cavemode) {
        for (int y = s.top; y > 0; y--) {
          bt = b_r.get8(x, z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }

      int hmval = hm_r.get8(x, z);
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        
        bt = b_r.get8(x, z, y);
        
        if (s.cavemode) {
          if (cavemode_ignore_block(s, x, z, y, bt, b_r, cave_initial) || y >= cavemode_top) {
            continue;
          }
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = bl_r.get4(x, z, y);
        
        int px, py;
        c.project_isometric(p, px, py);
        
        color top = mc::MaterialColor[bt];
        color side = mc::MaterialSideColor[bt];
        
        apply_shading(s, bl, sl_r.get4(x, z, y + 1), hmval, y, top);
        apply_shading(s, bl, -1, hmval, y, side);
        
        int bx, by;
        c.project_isometric(p, bx, by);
        
        int bp = bx + bmx * by;
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque();
          
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          oper->add_pixel(px - 2, py, top);
          oper->add_pixel(px - 1, py, top);
          
          oper->add_pixel(px - 2, py + 1, side);
          oper->add_pixel(px - 1, py + 1, side);
          
          oper->add_pixel(px - 2, py + 2, side);
          oper->add_pixel(px - 1, py + 2, side);
          
          side.lighten(0x20);
          
          oper->add_pixel(px, py + 1, side);
          oper->add_pixel(px + 1, py + 1, side);

          oper->add_pixel(px, py + 2, side);
          oper->add_pixel(px + 1, py + 2, side);
          break;
        case mc::HalfBlock:
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          oper->add_pixel(px, py + 1, top);
          oper->add_pixel(px + 1, py + 1, top);
          break;
        case mc::TopBlock:
          oper->add_pixel(px, py - 1, top);
          oper->add_pixel(px + 1, py - 1, top);
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          break;
        }
      }
    }
  }
  
  if (cache_use) {
    if (!cache.write(oper)) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
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
  std::string basename = fs::basename(path);
  boost::split(parts, basename, boost::is_any_of("."));
  
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

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

/**
 * Blocks[ z + ( y * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
nbt::Byte bget(nbt::ByteArray *blocks, int x, int z, int y) {
  
  
  
  int p = y + (z * mc::MapY + (x * mc::MapY * mc::MapZ));
  assert (p >= 0 && p < blocks->length);
  return blocks->values[p];
}

nbt::Byte bsget(nbt::ByteArray *skylight, int x, int z, int y) {
  
  
  
  int p = y + (z * mc::MapY + (x * mc::MapY * mc::MapZ));
  int ap = p / 2;

  assert (ap >= 0 && ap < skylight->length);
  
  // force unsigned
  uint8_t bp = skylight->values[ap] & 0xff;
  
  if (p % 2 == 0) {
    return bp >> 4;
  }
  else {
    return bp & 0xf;
  }
}

inline void apply_shading(settings_t& s, int bl, int sl, int hm, int y, color &c) {
  // if night, darken all colors not emitting light
  if (s.night) {
    c.darken((0xb0 * (16 - bl)) / 16);
  }

  c.darken(0x02 * sl);
  
  c.darken((mc::MapY - y));

  // in heightmap mode, brightness = height
  if (s.heightmap) {
    c.b = y*2;
    c.g = y*2;
    c.r = y*2;
    c.a = 0xff;
  }
}

inline bool cavemode_isopen(int bt) {
  switch(bt) {
    case mc::Air: return true;
    case mc::Leaves: return true;
    default: return false;
  }
}

inline bool cavemode_ignore_block(settings_t& s, int x, int z, int y, int bt, nbt::ByteArray *blocks, bool &cave_initial) {
  if (cave_initial) {
    if (!cavemode_isopen(bt)) {
      cave_initial = false;
      return true;
    }
    
    return true;
  }
  
  if (!cavemode_isopen(bt)) {
    if (y < s.top && cavemode_isopen(bget(blocks, x, z, y + 1))) {
      return false;
    }
  }

  return true;
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
  
  uint8_t get4(int x, int z, int y) {
    transform_xz(x, z);
    
    int p = y + (z * mc::MapY + (x * mc::MapY * mc::MapZ));
    int ap = p / 2;

    assert (ap >= 0 && ap < byte_array->length);
    
    // force unsigned
    uint8_t bp = byte_array->values[ap] & 0xff;
    
    if (p % 2 == 0) {
      return bp >> 4;
    }
    else {
      return bp & 0xf;
    }
  }
};

image_operations* level_file::get_image(settings_t& s) {
  if (cache_hit) return oper;
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  if (!islevel) {
    return oper;
  }
  
  // block type
  BlockRotation blocks_r(s, blocks);
  BlockRotation blocklight_r(s, blocklight);
  BlockRotation skylight_r(s, skylight);
  
  for (int z = mc::MapZ - 1; z >= 0; z--) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;
      
      // do incremental color fill until color is opaque
      for (int y = s.top; y > s.bottom; y--) {
        int bt = blocks_r.get8(x, z, y);
        
        if (s.cavemode && cavemode_ignore_block(s, x, z, y, bt, blocks, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        color bc = mc::MaterialColor[bt];
        
        int bl = blocklight_r.get4(x, z, y),
            sl = skylight_r.get4(x, z, y);
        
        apply_shading(s, bl, sl, 0, y, bc);
        
        point p(mc::MapZ - z - 1, y, x);

        int px, py;

        c.project_top(p, px, py);
        
        oper->add_pixel(px, py, bc);
        
        if (bc.is_opaque()) {
          break;
        }
      }
    }
  }

  oper->optimize();
  
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
      
  BlockRotation blocks_r(s, blocks);
  BlockRotation blocklight_r(s, blocklight);
  BlockRotation skylight_r(s, skylight);
  
  for (int x = 0, mz = mc::MapZ - 1; x < mc::MapX; x++, mz--) {
    for (int y = 0, mx = 0; y < mc::MapX; y++, mx++) {
      bool cave_initial = true;
      int cavemode_top = s.top;
      
      if (s.cavemode) {
        for (int my = s.top; my > 0; my--) {
          bt = blocks_r.get8(mx, mz, my);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = my;
            break;
          }
        }
      }

      for (int my = s.bottom; my <= s.top; my++) {
        point p(x, my, y);
        
        bt = blocks_r.get8(mx, mz, my);
        
        if (s.cavemode) {
          if (cavemode_ignore_block(s, mx, mz, my, bt, blocks, cave_initial) || my >= cavemode_top) {
            continue;
          }
        }
        
        if (s.excludes[bt]) {
          continue;
        }

        int bl = blocklight_r.get4(mx, mz, my),
            sl = skylight_r.get4(mx, mz, my);
        
        if (my + 1 <= s.top && mx + 1 < mc::MapX) {
          int checkblock = blocks_r.get8(mx + 1, mz, my + 1);
          if (checkblock != mc::Air && mc::MaterialColor[checkblock].is_opaque()) {
            continue;
          }
        }
        
        int px, py;
        c.project_oblique(p, px, py);
        
        color top = mc::MaterialColor[bt];
        apply_shading(s, bl, sl, 0, my, top);
        oper->add_pixel(px, py - 1, top);
        
        color side = mc::MaterialSideColor[bt];
        apply_shading(s, bl, sl, 0, my, side);
        oper->add_pixel(px, py, side);
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
      
  BlockRotation blocks_r(s, blocks);
  BlockRotation blocklight_r(s, blocklight);
  BlockRotation skylight_r(s, skylight);
  BlockRotation heightmap_r(s, heightmap);

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
          bt = blocks_r.get8(x, z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }

      int hmval = heightmap_r.get8(x, z);
      
      for (int y = s.bottom; y <= s.top; y++) {
        point p(x, y, z);
        
        bt = blocks_r.get8(x, z, y);
        
        if (s.cavemode) {
          if (cavemode_ignore_block(s, x, z, y, bt, blocks, cave_initial) || y >= cavemode_top) {
            continue;
          }
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = blocklight_r.get4(x, z, y),
            sl = skylight_r.get4(x, z, y);
        
        int px, py;
        c.project_obliqueangle(p, px, py);
        
        color top = mc::MaterialColor[bt];
        color side = mc::MaterialSideColor[bt];
        
        apply_shading(s, bl, sl, hmval, y, top);
        apply_shading(s, bl, sl, hmval, y, side);
        
        int bx, by;
        c.project_obliqueangle(p, bx, by);
        
        int bp = bx + bmx * by;
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque();
          oper->add_pixel(px + 1, py - 1, top);
          oper->add_pixel(px + 1, py, side);
          
          top.darken(0x20);
          side.darken(0x20);
          oper->add_pixel(px, py - 1, top);
          oper->add_pixel(px, py, side);
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

  oper->optimize();
  
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
      
  BlockRotation blocks_r(s, blocks);
  BlockRotation blocklight_r(s, blocklight);
  BlockRotation skylight_r(s, skylight);
  BlockRotation heightmap_r(s, heightmap);
  
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
          bt = blocks_r.get8(x, z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }

      int hmval = heightmap_r.get8(x, z);
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        
        bt = blocks_r.get8(x, z, y);
        
        if (s.cavemode) {
          if (cavemode_ignore_block(s, x, z, y, bt, blocks, cave_initial) || y >= cavemode_top) {
            continue;
          }
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = blocklight_r.get4(x, z, y),
            sl = skylight_r.get4(x, z, y);
        
        int px, py;
        c.project_isometric(p, px, py);
        
        color top = mc::MaterialColor[bt];
        color side = mc::MaterialSideColor[bt];
        
        apply_shading(s, bl, sl, hmval, y, top);
        apply_shading(s, bl, sl, hmval, y, side);
        
        int bx, by;
        c.project_isometric(p, bx, by);
        
        int bp = bx + bmx * by;
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque();
          
          oper->add_pixel(px - 2, py, top);
          oper->add_pixel(px - 1, py, top);
          
          oper->add_pixel(px - 2, py + 1, side);
          oper->add_pixel(px - 1, py + 1, side);
          
          oper->add_pixel(px - 2, py + 2, side);
          oper->add_pixel(px - 1, py + 2, side);

          oper->add_pixel(px - 1, py + 2, side);
          
          top.lighten(0x20);
          side.lighten(0x20);
          
          oper->add_pixel(px, py, top);
          oper->add_pixel(px + 1, py, top);
          
          oper->add_pixel(px, py + 1, side);
          oper->add_pixel(px + 1, py + 1, side);

          oper->add_pixel(px, py + 2, side);
          oper->add_pixel(px + 1, py + 2, side);
          
          oper->add_pixel(px, py + 2, side);
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
  
  oper->optimize();
  
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

fast_level_file::fast_level_file(const fs::path path, bool filename)
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
  
  if (filename) {
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

    xPos = common::b36decode(x);
    zPos = common::b36decode(z);
    islevel = true;
  }
  else {
    if (extension.compare(".dat") != 0) {
      grammar_error = true;
      grammar_error_why = "File extension is not .dat";
      return;
    }

    parser.register_int = fast_register_int;
    parser.begin_compound = fast_begin_compound;
    parser.error_handler = fast_error_handler;
    
    parser.parse_file(path.string().c_str());
  }
}

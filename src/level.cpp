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

level_file::level_file(settings_t& s)
    :
    islevel(false),
    grammar_error(false),
    grammar_error_where(0),
    grammar_error_why(""),
    in_te(false), in_sign(false),
    sign_x(0), sign_y(0), sign_z(0),
    sign_text(""),
    cache(s.cache_dir, s.cache_compress),
    cache_use(s.cache_use),
    cache_hit(false),
    oper(new image_operations)
{ }

void level_file::load_file(const fs::path path) {
  if (cache_use) {
    cache.set_path(fs::basename(path) + ".cmap" );
    
    std::time_t level_mod = fs::last_write_time(path);
  
    if (fs::exists(cache.get_path())) {
      if (cache.read(oper.get(), level_mod)) {
        cache_hit = true;
        islevel = true;
        return;
      }
      
      fs::remove(cache.get_path());
    }
    
    // in case of future writes, save the modification time
    cache.set_modification_time(level_mod);
  }
  
  nbt::Parser<level_file> parser(this);
  
  parser.register_byte_array = register_byte_array;
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

  int x, z;

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

  void set_xz(int x, int z) {
    transform_xz(x, z);
    this->x = x;
    this->z = z;
  }
  
  uint8_t get8(int y) {
    int p = y + (z * mc::MapY) + (x * mc::MapY * mc::MapZ);
    assert (p >= 0 && p < byte_array->length);
    return byte_array->values[p];
  }
  
  uint8_t get8() {
    int p = x + (z * mc::MapX);
    assert (p >= 0 && p < byte_array->length);
    return byte_array->values[p];
  }
  
  int get4(int y) {
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

inline bool cave_isopen(int bt) {
  switch(bt) {
    case mc::Air: return true;
    case mc::Leaves: return true;
    default: return false;
  }
}

inline bool cave_ignore_block(settings_t& s, int y, int bt, BlockRotation& b_r, bool &cave_initial) {
  if (cave_initial) {
    if (!cave_isopen(bt)) {
      cave_initial = false;
      return true;
    }
    
    return true;
  }
  
  if (!cave_isopen(bt)) {
    if (y < s.top && cave_isopen(b_r.get8(y + 1))) {
      return false;
    }
  }

  return true;
}

boost::shared_ptr<image_operations> level_file::get_image(settings_t& s) {
  if (cache_hit) return oper;
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  if (!islevel) {
    return oper;
  }
  
  // block type
  BlockRotation b_r(s, blocks.get());
  BlockRotation bl_r(s, blocklight.get());
  BlockRotation sl_r(s, skylight.get());
  
  int bx, by;

  c.get_top_limits(bx, by);

  oper->set_limits(bx + 1, by);
  
  for (int z = mc::MapZ - 1; z >= 0; z--) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;

      b_r.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      // do incremental color fill until color is opaque
      for (int y = s.top; y > s.bottom; y--) {
        int bt = b_r.get8(y);
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        color bc = mc::MaterialColor[bt];
        
        apply_shading(s, bl_r.get4(y), sl_r.get4(y + 1), 0, y, bc);
        
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
    if (!cache.write(oper.get())) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
}

boost::shared_ptr<image_operations> level_file::get_oblique_image(settings_t& s)
{
  if (cache_hit) return oper;
  
  if (!islevel) {
    return oper;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  // block type
      
  BlockRotation b_r(s, blocks.get());
  BlockRotation bl_r(s, blocklight.get());
  BlockRotation sl_r(s, skylight.get());
  
  int bmx, bmy, bmt;
  c.get_oblique_limits(bmx, bmy);
  bmt = bmx * bmy;
  bool blocked[bmt];
  memset(blocked, 0x0, sizeof(bool) * bmt);
  
  oper->set_limits(bmx + 1, bmy);
  
  for (int z = c.z - 1; z >= 0; z--) {
    for (int x = c.x - 1; x >= 0; x--) {
      bool cave_initial = true;

      b_r.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        
        int bt = b_r.get8(y);
        
        int px, py;
        c.project_oblique(p, px, py);
        
        color top = mc::MaterialColor[bt];
        
        int bp = px + bmx * py;
        
        if (blocked[bp]) {
          continue;
        }
        
        blocked[bp] = top.is_opaque();
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = bl_r.get4(y);
        
        apply_shading(s, bl, sl_r.get4(y + 1), 0, y, top);
        oper->add_pixel(px, py, top);
        
        color side = mc::MaterialSideColor[bt];
        apply_shading(s, bl, -1, 0, y, side);
        oper->add_pixel(px, py + 1, side);
      }
    }
  }
  
  if (cache_use) {
    if (!cache.write(oper.get())) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
}
boost::shared_ptr<image_operations> level_file::get_obliqueangle_image(settings_t& s)
{
  if (cache_hit) return oper;
  
  if (!islevel) {
    return oper;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  // block type
  
  BlockRotation b_r(s, blocks.get());
  BlockRotation bl_r(s, blocklight.get());
  BlockRotation sl_r(s, skylight.get());
  BlockRotation hm_r(s, heightmap.get());

  int bmx, bmy, bmt;
  c.get_obliqueangle_limits(bmx, bmy);
  bmt = bmx * bmy;
  bool blocked[bmt];
  memset(blocked, 0x0, sizeof(bool) * bmt);
  
  oper->set_limits(bmx + 1, bmy);
  
  for (int z = c.z - 1; z >= 0; z--) {
    for (int x = c.x - 1; x >= 0; x--) {
      bool cave_initial = true;
      
      hm_r.set_xz(x, z);
      b_r.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      int hmval = hm_r.get8();
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        
        int px, py;
        c.project_obliqueangle(p, px, py);
        
        int bt = b_r.get8(y);
        
        color top = mc::MaterialColor[bt];
        
        if (mc::MaterialModes[bt] == mc::Block) {
          int bp = px + bmx * py;
          
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque();
        }
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        int bl = bl_r.get4(y);
        
        color side = mc::MaterialSideColor[bt];
        
        apply_shading(s, bl, sl_r.get4(y + 1), hmval, y, top);
        apply_shading(s, bl, -1, hmval, y, side);
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
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
    if (!cache.write(oper.get())) {
      fs::remove(cache.get_path());
    }
  }
  
  return oper;
}

boost::shared_ptr<image_operations> level_file::get_isometric_image(settings_t& s)
{
  if (cache_hit) return oper;
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);

  int iw, ih;
  c.get_isometric_limits(iw, ih);
  
  if (!islevel) {
    return oper;
  }
  
  // block type
  BlockRotation b_r(s, blocks.get());
  BlockRotation bl_r(s, blocklight.get());
  BlockRotation sl_r(s, skylight.get());
  BlockRotation hm_r(s, heightmap.get());
  
  int bmt;
  bmt = iw * ih;
  bool blocked[bmt];
  memset(blocked, 0x0, sizeof(bool) * bmt);

  oper->set_limits(iw + 1, ih);
  
  for (int z = c.z - 1; z >= 0; z--) {
    for (int x = c.x - 1; x >= 0; x--) {
      bool cave_initial = true;
      
      hm_r.set_xz(x, z);
      b_r.set_xz(x, z);
      bl_r.set_xz(x, z);
      sl_r.set_xz(x, z);
      
      int hmval = hm_r.get8();
      
      for (int y = s.top; y >= s.bottom; y--) {
        point p(x, y, z);
        int bt = b_r.get8(y);
        color top = mc::MaterialColor[bt];
        
        int px, py;
        c.project_isometric(p, px, py);
        
        if (mc::MaterialModes[bt] == mc::Block) {
          int bp = px + iw * py;
          
          if (blocked[bp]) {
            continue;
          }
          
          blocked[bp] = top.is_opaque();
        }
        
        if (s.cavemode && cave_ignore_block(s, y, bt, b_r, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        color side = mc::MaterialSideColor[bt];
        
        int bl = bl_r.get4(y);
        
        apply_shading(s, bl, sl_r.get4(y + 1), hmval, y, top);
        apply_shading(s, bl, -1, hmval, y, side);
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
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
    if (!cache.write(oper.get())) {
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

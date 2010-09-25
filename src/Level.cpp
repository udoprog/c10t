#include <assert.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>

#include "global.h"

#include "Level.h"
#include "blocks.h"
#include "fileutils.h"
#include "2d/cube.h"

void begin_compound(void *context, nbt::String name) {
  if (name.compare("Level") == 0) {
    ((Level*)context)->islevel = true;
  }
}

void register_int(void *context, nbt::String name, nbt::Int i) {
  Level *level = ((Level*)context);

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

void register_byte_array(void *context, nbt::String name, nbt::ByteArray* byte_array) {
  Level *level = ((Level*)context);
  
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

void error_handler(void *context, size_t where, const char *why) {
  Level *level = ((Level *)context);
  level->grammar_error = true;
  level->grammar_error_where = where;
  level->grammar_error_why = why;
}

#include <iostream>

Level::~Level(){
  unload_data();
  assert(load_count == 0);
}

Level::Level(settings_t& s, const std::string& path)
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
    load_count(0),
    load_tried(false)
{
  if (parse_filename_coordinates(path, xPos, zPos)) {
    transform_world_xz(xPos, zPos, s.rotation);
    islevel = true;
  }
}

/**
 * Blocks[ z + ( y * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
nbt::Byte bget(nbt::ByteArray *blocks, int x, int z, int y) {
  assert(x >= 0 && x < mc::MapX);
  assert(z >= 0 && z < mc::MapZ);
  assert(y >= 0 && y < mc::MapY);
  int p = y + (z * mc::MapY + (x * mc::MapY * mc::MapZ));
  assert (p >= 0 && p < blocks->length);
  return blocks->values[p];
}

nbt::Byte bsget(nbt::ByteArray *skylight, int x, int z, int y) {
  assert(x >= 0 && x < mc::MapX);
  assert(z >= 0 && z < mc::MapZ);
  assert(y >= 0 && y < mc::MapY);
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

inline void apply_shading(settings_t& s, int bl, int sl, int hm, int y, Color &c) {
  // if night, darken all colors not emitting light
  if (s.night) {
    c.darken((0xd0 * (16 - bl)) / 16);
  }

  c.darken(0x02 * sl);
  
  c.darken((mc::MapY - y));
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


bool parse_filename_coordinates(const std::string& path, int& x, int& z)
{
  // Make sure the path contains X and Z subdirectories and the filename.
  std::vector<std::string> subpaths = path_split(path);
  if (subpaths.size() < 3) {
    return false;
  }

  // Make sure the filename has the form "c.(X).(Z).dat"
  std::vector<std::string> parts;
  boost::split(parts, subpaths.back(), boost::is_any_of("."));
  if (   parts.size() != 4
      || !boost::iequals(parts[0], "c")
      || !boost::iequals(parts[3], "dat")) {
    return false;
  }

  // Convert base36 values from the filename and directories.
  long xFile = strtol(parts[1].c_str(), NULL, 36);
  long zFile = strtol(parts[2].c_str(), NULL, 36);
  long xDir = strtol(subpaths[subpaths.size() - 3].c_str(), NULL, 36);
  long zDir = strtol(subpaths[subpaths.size() - 2].c_str(), NULL, 36);

  // Verify that (directory coordinates) == (file coordinates) mod 64.
  if (xDir != (xFile & 0x3f) || zDir != (zFile & 0x3f)) {
    return false;
  }

  x = xFile;
  z = zFile;

  return true;
}


void transform_world_xz(int& x, int& z, int rotation)
{
  int t = x;
  switch (rotation) {
    case 270:
      x = z;
      z = -t;
      break;
    case 180:
      x = -x;
      z = -z;
      break;
    case 90:
      x = -z;
      z = t;
      break;
  }
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
    
    assert(x >= 0 && x < mc::MapX);
    assert(z >= 0 && z < mc::MapZ);
    assert(y >= 0 && y < mc::MapY);
    int p = y + (z * mc::MapY + (x * mc::MapY * mc::MapZ));
    assert (p >= 0 && p < byte_array->length);
    return byte_array->values[p];
  }
  
  uint8_t get8(int x, int z) {
    transform_xz(x, z);
    
    assert(x >= 0 && x < mc::MapX);
    assert(z >= 0 && z < mc::MapZ);
    int p = x + (z * mc::MapX);
    assert (p >= 0 && p < byte_array->length);
    return byte_array->values[p];
  }
  
  uint8_t get4(int x, int z, int y) {
    transform_xz(x, z);
    
    assert(x >= 0 && x < mc::MapX);
    assert(z >= 0 && z < mc::MapZ);
    assert(y >= 0 && y < mc::MapY);
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

ImageBuffer *Level::get_image(settings_t& s) {
  ImageBuffer *img = new ImageBuffer(mc::MapX, mc::MapZ, 1);
  
  if (!islevel || grammar_error || !load_count) {
    return img;
  }

  img->set_reversed(true);
  
  // block type
  int bt;
  
  for (int x = 0, mz = mc::MapZ - 1; x < mc::MapX; x++, mz--) {
    for (int y = 0, mx = 0; y < mc::MapX; y++, mx++) {
      Color base(255, 255, 255, 0);
      
      bt = mc::Air;
      
      bool cave_initial = true;

      int my;
      
      BlockRotation blocks_r(s, blocks);
      BlockRotation blocklight_r(s, blocklight);
      BlockRotation skylight_r(s, skylight);
      
      // do incremental color fill until color is opaque
      for (my = s.top; my > s.bottom; my--) {
        bt = blocks_r.get8(mx, mz, my);
        
        if (s.cavemode && cavemode_ignore_block(s, mx, mz, my, bt, blocks, cave_initial)) {
          continue;
        }
        
        if (s.excludes[bt]) {
          continue;
        }
        
        Color bc(mc::MaterialColor[bt]);
        
        int bl = blocklight_r.get4(mx, mz, my),
            sl = skylight_r.get4(mx, mz, my);
        
        apply_shading(s, bl, sl, 0, my, bc);
        
        base.underlay(bc);
        
        if (base.is_opaque()) {
          break;
        }
      }

      if (base.is_transparent()) {
        continue;
      }
      
      img->set_pixel(x, y, 0, base);
    }
  }
  
  return img;
}

ImageBuffer *Level::get_oblique_image(settings_t& s)
{
  ImageBuffer *img = new ImageBuffer(mc::MapX, mc::MapZ + mc::MapY, mc::MapY + mc::MapZ);
  
  if (!islevel || grammar_error || !load_count) {
    return img;
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
          bt = blocks_r.get8(mx, mz, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = my;
            break;
          }
        }
      }

      for (int my = s.bottom; my < s.top; my++) {
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
        
        int px, py;
        c.project_oblique(p, px, py);
        
        Color top(mc::MaterialColor[bt]);
        apply_shading(s, bl, sl, 0, my, top);
        img->add_pixel(px, py - 1, top);
        
        Color side(mc::MaterialSideColor[bt]);
        apply_shading(s, bl, sl, 0, my, side);
        img->add_pixel(px, py, side);
      }
    }
  }
  
  return img;
}

ImageBuffer *Level::get_obliqueangle_image(settings_t& s)
{
  ImageBuffer *img = new ImageBuffer(mc::MapX * 2 + 1, mc::MapX + mc::MapY + mc::MapZ, mc::MapY + mc::MapZ * 2);
  
  if (!islevel || grammar_error || !load_count) {
    return img;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  // block type
  int bt;
      
  BlockRotation blocks_r(s, blocks);
  BlockRotation blocklight_r(s, blocklight);
  BlockRotation skylight_r(s, skylight);
  BlockRotation heightmap_r(s, heightmap);
  
  for (int z = 0; z < c.z; z++) {
    for (int x = 0; x < c.x; x++) {
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
      
      for (int y = s.bottom; y < s.top; y++) {
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
        
        Color top(mc::MaterialColor[bt]);
        Color side(mc::MaterialSideColor[bt]);
        
        apply_shading(s, bl, sl, hmval, y, top);
        apply_shading(s, bl, sl, hmval, y, side);
        
        switch(mc::MaterialModes[bt]) {
        case mc::Block:
          img->add_pixel(px, py - 1, top);
          img->add_pixel(px + 1, py - 1, top);
          img->add_pixel(px, py, side);
          side.darken(0x20);
          img->add_pixel(px + 1, py, side);
          break;
        case mc::HalfBlock:
          img->add_pixel(px, py, top);
          img->add_pixel(px + 1, py, top);
          break;
        case mc::TopBlock:
          img->add_pixel(px, py - 1, top);
          img->add_pixel(px + 1, py - 1, top);
          break;
        }
      }
    }
  }
  
  return img;
}


void Level::load_data(settings_t& s)
{
  boost::mutex::scoped_lock lock(load_mutex);

  if (load_count == 0 && !load_tried) {
    load_tried = true;
    islevel = false; // Need to verify that the file is valid, not just filename.

    // Save original values based on filename to verify after loading.
    int xOld = xPos;
    int zOld = zPos;

    nbt::Parser parser(this);
    parser.register_byte_array = register_byte_array;
    parser.register_int = register_int;
    parser.begin_compound = begin_compound;
    parser.error_handler = error_handler;

    try {
      parser.parse_file(path.c_str());
    } catch(nbt::bad_grammar &bg) {
      grammar_error = true;
    }

    if (islevel && !grammar_error) {
      transform_world_xz(xPos, zPos, s.rotation);
      if (xOld != xPos || zOld != zPos) {
        grammar_error = true;
        grammar_error_where = 0;
        grammar_error_why = "level coordinate mismatch";
      }
    }
  }

  ++load_count;
}

void Level::unload_data()
{
  boost::mutex::scoped_lock lock(load_mutex);

  if (load_count == 1) {
    delete blocks;
    blocks = NULL;

    delete skylight;
    skylight = NULL;

    delete heightmap;
    heightmap = NULL;

    delete blocklight;
    blocklight = NULL;
  }

  if (load_count > 0) {
    --load_count;
  }
}

#include <assert.h>

#include "global.h"

#include "Level.h"
#include "blocks.h"
#include "2d/cube.h"

void begin_compound(void *context, nbt::String name) {
  if (name.compare("Level") == 0) {
    ((Level*)context)->islevel = true;
  }
}

void register_int(void *context, nbt::String name, nbt::Int i) {
  if (name.compare("xPos") == 0) {
    ((Level *)context)->xPos = i;
    return;
  }
  
  if (name.compare("zPos") == 0) {
    ((Level *)context)->zPos = i;
    return;
  }
}

void register_byte_array(void *context, nbt::String name, nbt::ByteArray *byte_array) {
  if (name.compare("Blocks") == 0) {
    ((Level *)context)->blocks = byte_array;
    return;
  }

  if (name.compare("SkyLight") == 0) {
    ((Level *)context)->skylight = byte_array;
    return;
  }

  if (name.compare("HeightMap") == 0) {
    ((Level *)context)->heightmap = byte_array;
    return;
  }

  if (name.compare("BlockLight") == 0) {
    ((Level *)context)->blocklight = byte_array;
    return;
  }
}

void error_handler(void *context, size_t where, const char *why) {
  ((Level *)context)->grammar_error = true;
  ((Level *)context)->grammar_error_where = where;
  ((Level *)context)->grammar_error_why = why;
}

Level::~Level(){
  if (!ignore_blocks && islevel) {
    delete blocks;
    delete skylight;
    delete heightmap;
    delete blocklight;
  }
}

Level::Level(const char *path, bool ignore_blocks)
  : path(path), ignore_blocks(ignore_blocks)
{
  xPos = 0;
  zPos = 0;
  islevel = false;
  grammar_error = false;
  
  nbt::Parser parser(this);
  
  if (!ignore_blocks) {
    parser.register_byte_array = register_byte_array;
  }
  
  parser.register_int = register_int;
  parser.begin_compound = begin_compound;
  parser.error_handler = error_handler;
  
  try {
    parser.parse_file(path);
  } catch(nbt::bad_grammar &bg) {
    grammar_error = true;
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

void transform_xz(settings_t *s, int &x, int &z) {
  if (s->flip) {
    int t = x;
    x = z; 
    z = mc::MapZ - t - 1;
  }
  
  if (s->invert) {
    z = mc::MapZ - z - 1;
    x = mc::MapX - x - 1;
  }
}

inline void apply_shading(settings_t *s, int bl, int sl, int hm, Color &c) {
  // if night, darken all colors not emitting light
  if (s->night) {
    c.darken((0xd0 * (16 - bl)) / 16);
  }
  
  c.darken(sl);
}

inline bool cavemode_isopen(int bt) {
  switch(bt) {
    case mc::Air: return true;
    case mc::Leaves: return true;
    default: return false;
  }
}

inline bool cavemode_ignore_block(settings_t *s, int x, int z, int y, int bt, nbt::ByteArray *blocks, bool &cave_initial) {
  if (cave_initial) {
    if (!cavemode_isopen(bt)) {
      cave_initial = false;
      return true;
    }
    
    return true;
  }
  
  if (!cavemode_isopen(bt)) {
    if (y < s->top && cavemode_isopen(bget(blocks, x, z, y + 1))) {
      return false;
    }
  }

  return true;
}

Image *Level::get_image(settings_t *s) {
  Image *img = new Image(mc::MapX, mc::MapZ);
  
  if (!islevel) {
    return img;
  }
  
  // block type
  int bt;
  
  for (int z = 0; z < mc::MapZ; z++) {
    for (int x = 0; x < mc::MapX; x++) {
      int _x = x, _z = z;
      transform_xz(s, _x, _z);
      
      Color base(255, 255, 255, 0);
      
      bt = mc::Air;
      
      int y;
      
      bool cave_initial = true;
      
      // do incremental color fill until color is opaque
      for (y = s->top; y > s->bottom; y--) {
        bt = bget(blocks, _x, _z, y);
        
        if (s->cavemode && cavemode_ignore_block(s, _x, _z, y, bt, blocks, cave_initial)) {
          continue;
        }
        
        if (s->excludes[bt]) {
          continue;
        }
        
        Color *bc = mc::MaterialColor[bt];
        base.underlay(bc);
        
        if (base.is_opaque()) {
          break;
        }
      }
      
      if (base.is_transparent()) {
        continue;
      }
      
      int bl = bsget(blocklight, _x, _z, y);
      int sl = bsget(skylight, _x, _z, y);
      apply_shading(s, bl, sl, 0, base);
      
      img->set_pixel(x, z, base);
    }
  }
  
  return img;
}

Image *Level::get_oblique_image(settings_t *s) {
  Image *img = new Image(mc::MapX, mc::MapZ + mc::MapY);
  
  if (!islevel) {
    return img;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);

  // block type
  int bt;
  
  for (int z = 0; z < mc::MapZ; z++) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;
      int cavemode_top = s->top;
      
      if (s->cavemode) {
        for (int y = s->top; y > 0; y--) {
          int _x = x, _z = z;
          transform_xz(s, _x, _z);
          
          bt = bget(blocks, _x, _z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }

      for (int y = s->bottom; y < s->top; y++) {
        point p(x, y, z);
        int _x = x, _z = z, _y = y;
        transform_xz(s, _x, _z);
        bt = bget(blocks, _x, _z, _y);
        
        if (s->cavemode) {
          if (cavemode_ignore_block(s, _x, _z, _y, bt, blocks, cave_initial) || _y >= cavemode_top) {
            continue;
          }
        }
        
        if (s->excludes[bt]) {
          continue;
        }

        int bl = bsget(blocklight, _x, _z, _y);
        int sl = bsget(skylight, _x, _z, _y);
        
        int _px, _py;
        c.project_oblique(p, _px, _py);
        
        Color top(mc::MaterialColor[bt]);
        apply_shading(s, bl, sl, 0, top);
        img->set_pixel(_px, _py - 1, top);
        
        Color side(mc::MaterialSideColor[bt]);
        apply_shading(s, bl, sl, 0, side);
        img->set_pixel(_px, _py, side);
      }
    }
  }
  
  return img;
}

Image *Level::get_obliqueangle_image(settings_t *s) {
  Image *img = new Image(mc::MapX * 2 + 1, mc::MapX + mc::MapY + mc::MapZ);
  
  if (!islevel) {
    return img;
  }
  
  Cube c(mc::MapX, mc::MapY, mc::MapZ);
  
  // block type
  int bt;
  
  for (int z = 0; z < c.z; z++) {
    for (int x = 0; x < c.x; x++) {
      bool cave_initial = true;

      int cavemode_top = s->top;

      if (s->cavemode) {
        for (int y = s->top; y > 0; y--) {
          int _x = x, _z = z;
          transform_xz(s, _x, _z);
          
          bt = bget(blocks, _x, _z, y);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = y;
            break;
          }
        }
      }
      
      for (int y = s->bottom; y < s->top; y++) {
        point p(x, y, z);
        
        int _x = x, _z = z;
        transform_xz(s, _x, _z);
        
        bt = bget(blocks, _x, _z, y);
        
        if (s->cavemode) {
          if (cavemode_ignore_block(s, _x, _z, y, bt, blocks, cave_initial) || y >= cavemode_top) {
            continue;
          }
        }
        
        if (s->excludes[bt]) {
          continue;
        }
    
        int bl = bsget(blocklight, _x, _z, y);
        int sl = bsget(skylight, _x, _z, y);
        
        int _px, _py;
        c.project_obliqueangle(p, _px, _py);
        
        Color top(mc::MaterialColor[bt]);
        apply_shading(s, bl, sl, 0, top);
        img->set_pixel(_px, _py - 1, top);
        img->set_pixel(_px + 1, _py - 1, top);
        img->set_pixel(_px, _py - 2, top);
        img->set_pixel(_px + 1, _py - 2, top);
        
        Color side(mc::MaterialSideColor[bt]);
        apply_shading(s, bl, sl, 0, side);
        img->set_pixel(_px, _py, side);
        img->set_pixel(_px + 1, _py, side);
      }
    }
  }
  
  return img;
}

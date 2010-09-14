#include <assert.h>

#include "global.h"

#include "Level.h"
#include "blocks.h"

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
  if (islevel) {
    delete blocks;
    delete skylight;
    delete heightmap;
    delete blocklight;
  }
}

Level::Level(const char *path) {
  xPos = 0;
  zPos = 0;
  islevel = false;
  grammar_error = false;
  
  nbt::Parser parser(this);
  parser.register_byte_array = register_byte_array;
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
 * Blocks[ y + ( z * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
nbt::Byte bget(nbt::ByteArray *blocks, int x, int y, int z) {
  assert(x >= 0 && x < mc::MapX);
  assert(y >= 0 && y < mc::MapZ);
  assert(z >= 0 && z < mc::MapY);
  int p = z + (y * mc::MapY + (x * mc::MapY * mc::MapZ));
  assert (p >= 0 && p < blocks->length);
  return blocks->values[p];
}

nbt::Byte bsget(nbt::ByteArray *skylight, int x, int y, int z) {
  assert(x >= 0 && x < mc::MapX);
  assert(y >= 0 && y < mc::MapZ);
  assert(z >= 0 && z < mc::MapY);
  int p = z + (y * mc::MapY + (x * mc::MapY * mc::MapZ));
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

void transform_xy(settings_t *s, int &x, int &y) {
  if (s->flip) {
    int t = x;
    x = y; 
    y = mc::MapZ - t - 1;
  }
  
  if (s->invert) {
    y = mc::MapZ - y - 1;
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

inline bool cavemode_ignore_block(settings_t *s, int x, int y, int z, int bt, nbt::ByteArray *blocks, bool &cave_initial) {
  if (cave_initial) {
    if (!cavemode_isopen(bt)) {
      cave_initial = false;
      return true;
    }
    
    return true;
  }
  
  if (!cavemode_isopen(bt)) {
    if (z < s->top && cavemode_isopen(bget(blocks, x, y, z + 1))) {
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
  
  // night modifier
  Color nightmod(0x0, 0x0, 0x0, 200);
  
  for (int y = 0; y < mc::MapZ; y++) {
    for (int x = 0; x < mc::MapX; x++) {
      int _x = x, _y = y;
      transform_xy(s, _x, _y);
      
      Color base(255, 255, 255, 0);
      
      bt = mc::Air;
      
      int z;
      
      bool cave_initial = true;
      
      // do incremental color fill until color is opaque
      for (z = s->top; z > s->bottom; z--) {
        bt = bget(blocks, _x, _y, z);
        
        if (s->cavemode && cavemode_ignore_block(s, _x, _y, z, bt, blocks, cave_initial)) {
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
      
      int bl = bsget(blocklight, _x, _y, z);
      int sl = bsget(skylight, _x, _y, z);
      apply_shading(s, bl, sl, 0, base);
      
      img->set_pixel(x, y, base);
    }
  }
  
  return img;
}

Image *Level::get_oblique_image(settings_t *s) {
  Image *img = new Image(mc::MapX, mc::MapZ + mc::MapY);
  
  if (!islevel) {
    return img;
  }

  // block type
  int bt;
  
  // night modifier
  Color nightmod(0x0, 0x0, 0x0, 200);
  
  // blocklight modifier
  Color blmod(0xff, 0xea, 0x86, 0);
  
  // skylight modifier
  // alpha channel is calculated depending on skylight value
  Color slmod(255, 255, 255, 0);
  
  // height modifier
  // alpha channel is calculated depending on height
  Color heightmod(0, 0, 0, 0);
  
  for (int y = 0; y < mc::MapZ; y++) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;
      int cavemode_top = s->top;
      
      if (s->cavemode) {
        for (int z = s->top; z > 0; z--) {
          int _x = x, _y = y;
          transform_xy(s, _x, _y);
          
          bt = bget(blocks, _x, _y, z);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = z;
            break;
          }
        }
      }

      for (int z = s->bottom; z < s->top; z++) {
        int _x = x, _y = y, _z = z;
        transform_xy(s, _x, _y);
        bt = bget(blocks, _x, _y, _z);
        
        if (s->cavemode) {
          if (cavemode_ignore_block(s, _x, _y, _z, bt, blocks, cave_initial) || _z >= cavemode_top) {
            continue;
          }
        }
        
        if (s->excludes[bt]) {
          continue;
        }

        int bl = bsget(blocklight, _x, _y, _z);
        int sl = bsget(skylight, _x, _y, _z);
        
        // optimization, don't draw top of block
        //if (_z + 1 >= s->top || bget(blocks, _x, _y, _z + 1) == mc::Air) {
          Color top(mc::MaterialColor[bt]);
          apply_shading(s, bl, sl, 0, top);
          img->set_pixel(x, y + (mc::MapY - z) - 1, top);
        //}
        
        // optimization, don't draw side of block if it has neighbour
        //if (_y + 1 >= mc::MapZ || bget(blocks, _x, _y + 1, _z) == mc::Air) {
          Color side(mc::MaterialSideColor[bt]);
          apply_shading(s, bl, sl, 0, side);
          img->set_pixel(x, y + (mc::MapY - z), side);
        //}
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

  // block type
  int bt;
  
  // night modifier
  Color nightmod(0x0, 0x0, 0x0, 200);
  
  // blocklight modifier
  Color blmod(0xff, 0xea, 0x86, 0);
  
  // skylight modifier
  // alpha channel is calculated depending on skylight value
  Color slmod(255, 255, 255, 0);
  
  // height modifier
  // alpha channel is calculated depending on height
  Color heightmod(0, 0, 0, 0);
  
  for (int y = 0; y < mc::MapZ; y++) {
    for (int x = 0; x < mc::MapX; x++) {
      bool cave_initial = true;

      int cavemode_top = s->top;

      if (s->cavemode) {
        for (int z = s->top; z > 0; z--) {
          int _x = x, _y = y;
          transform_xy(s, _x, _y);
          
          bt = bget(blocks, _x, _y, z);
          
          if (!cavemode_isopen(bt)) {
            cavemode_top = z;
            break;
          }
        }
      }
      
      for (int z = s->bottom; z < s->top; z++) {
        int _x = x, _y = y;
        transform_xy(s, _x, _y);
        
        bt = bget(blocks, _x, _y, z);
        
        if (s->cavemode) {
          if (cavemode_ignore_block(s, _x, _y, z, bt, blocks, cave_initial) || z >= cavemode_top) {
            continue;
          }
        }
        
        if (s->excludes[bt]) {
          continue;
        }
    
        int bl = bsget(blocklight, _x, _y, z);
        int sl = bsget(skylight, _x, _y, z);
        
        int _px = mc::MapX + x - y;
        int _py = mc::MapY + x - z + y;
        
        // optimization, don't draw top of block if it's blocked
        //if (z + 1 >= s->top || bget(blocks, _x, _y, z + 1) == mc::Air) {
          Color top(mc::MaterialColor[bt]);
          apply_shading(s, bl, sl, 0, top);
          img->set_pixel(_px, _py - 1, top);
          img->set_pixel(_px + 1, _py - 1, top);
          img->set_pixel(_px, _py - 2, top);
          img->set_pixel(_px + 1, _py - 2, top);
        //}
        
        // optimization, don't draw side of block if it has a neighbour
        //if (_x + 1 >= mc::MapX || _y + 1 >= mc::MapZ || bget(blocks, _x + 1, _y + 1, z) == mc::Air) {
          Color side(mc::MaterialSideColor[bt]);
          apply_shading(s, bl, sl, 0, side);
          img->set_pixel(_px, _py, side);
          img->set_pixel(_px + 1, _py, side);
        //}
      }
    }
  }
  
  return img;
}

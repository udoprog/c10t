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
  
  nbt::Parser parser(this);
  parser.register_byte_array = register_byte_array;
  parser.register_int = register_int;
  parser.begin_compound = begin_compound;
  parser.parse_file(path);
  
  if (islevel) {
  }
}

/**
 * Blocks[ y + ( z * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
nbt::Byte bget(nbt::ByteArray *blocks, int x, int y, int z) {
  assert(x >= 0 && x < 16);
  assert(y >= 0 && y < 16);
  assert(z >= 0 && z < 128);
  int p = z + (y * 128 + (x * 128 * 16));
  assert (p >= 0 && p < blocks->length);
  return blocks->values[p];
}

Image *Level::get_image(settings_t *s) {
  Image *img = new Image(16, 16);
  
  if (!islevel) {
    return img;
  }
  
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      Color base(255, 255, 255, 0);
      
      int blocktype;
      int z;
      
      // do incremental color fill until color is opaque
      for (z = s->top; z > s->bottom; z--) {
        blocktype = bget(blocks, x, y, z);
        
        if (s->exclude) {
          if (s->excludes[blocktype]) {
            continue;
          }
        }
        
        Color *bc = mc::MaterialColor[blocktype];
        base.underlay(*bc);
        
        if (base.a == 0xff) {
          break;
        }
      }
      
      // check specific last block options
      switch (blocktype) {
        case mc::Dirt:
        case mc::Grass:
        case mc::Stone:
        case mc::Cobblestone:
          // do an color overlay for mapped height
          Color height(0, 0, 0, 0);
          height.a = (127 - z);
          base.overlay(height);
          break;
      }
      
      img->set_pixel(x, y, base);
    }
  }
  
  return img;
}

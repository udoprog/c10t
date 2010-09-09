#include <assert.h>

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

void register_byte_array(void *context, nbt::String name, nbt::ByteArray byte_array) {
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
}

Level::Level(const char *path) {
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
nbt::Byte bget(nbt::ByteArray blocks, int x, int y, int z) {
  assert(x >= 0 && x < 16);
  assert(y >= 0 && y < 16);
  assert(z >= 0 && z < 128);
  int p = z + (y * 128 + (x * 128 * 16));
  assert (p >= 0 && p < blocks.length);
  return blocks.values[p];
}

Image Level::get_image() {
  Image img(16, 16);

  if (!islevel) {
    return img;
  }

  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      Color base(255, 255, 255, 255);
      
      for (int z = 0; z < 128; z++) {
        int blocktype = bget(blocks, x, y, z);
        Color bc = blockcolors[blocktype];
        base.overlay(bc);
      }

      img.set_pixel(x, y, base);
    }
  }
  
  return img;
}

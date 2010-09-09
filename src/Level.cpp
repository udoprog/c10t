#include <assert.h>

#include "Level.h"

struct nbtctx {
  nbt::ByteArray blocks;
  nbt::ByteArray skylight;
  nbt::ByteArray heightmap;
  nbt::ByteArray blocklight;
  nbt::Int xPos;
  nbt::Int zPos;
  bool islevel;
};

void begin_compound(void *context, nbt::String name) {
  if (name.compare("Level") == 0) {
    ((nbtctx *)context)->islevel = true;
  }
}

void register_int(void *context, nbt::String name, nbt::Int i) {
  if (name.compare("xPos") == 0) {
    ((nbtctx *)context)->xPos = i;
    return;
  }
  
  if (name.compare("zPos") == 0) {
    ((nbtctx *)context)->zPos = i;
    return;
  }
}

void register_byte_array(void *context, nbt::String name, nbt::ByteArray byte_array) {
  if (name.compare("Blocks") == 0) {
    ((nbtctx *)context)->blocks = byte_array;
    return;
  }

  if (name.compare("SkyLight") == 0) {
    ((nbtctx *)context)->skylight = byte_array;
    return;
  }

  if (name.compare("HeightMap") == 0) {
    ((nbtctx *)context)->heightmap = byte_array;
    return;
  }

  if (name.compare("BlockLight") == 0) {
    ((nbtctx *)context)->blocklight = byte_array;
    return;
  }
}

Level::~Level(){
}

Level::Level(const char *path) {
  
  nbtctx context;
  
  nbt::Parser parser(&context);
  parser.register_byte_array = register_byte_array;
  parser.register_int = register_int;
  parser.begin_compound = begin_compound;
  parser.parse_file(path);

  if (context.islevel) {
    this->blocks = context.blocks;
  }
}

/**
 * Blocks[ y + ( z * ChunkSizeY(=128) + ( x * ChunkSizeY(=128) * ChunkSizeZ(=16) ) ) ]; 
 */
nbt::Byte bget(nbt::Byte *blocks, int x, int y, int z) {
  assert(x >= 0 && x < 16);
  assert(y >= 0 && y < 16);
  assert(z >= 0 && z < 128);
  return blocks[y + (z * 128) + (x * 128 * 16)];
}

Image Level::get_image() {
  Image img(16, 16);
  img.set_pixel(0, 0, Color(0, 0, 0, 255));
  return img;
}

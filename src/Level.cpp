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
  assert(x >= 0 && x < mc::MapX);
  assert(y >= 0 && y < mc::MapY);
  assert(z >= 0 && z < mc::MapZ);
  int p = z + (y * mc::MapZ + (x * mc::MapZ * mc::MapY));
  assert (p >= 0 && p < blocks->length);
  return blocks->values[p];
}

Image *Level::get_image(settings_t *s) {
  Image *img = new Image(mc::MapX, mc::MapY);
  
  if (!islevel) {
    return img;
  }
  
  for (int x = 0; x < mc::MapX; x++) {
    for (int y = 0; y < mc::MapY; y++) {
      Color *base = new Color(255, 255, 255, 0);
      
      int blocktype;
      int z;
      
      // do incremental color fill until color is opaque
      for (z = s->top; z > s->bottom; z--) {
        blocktype = bget(blocks, x, y, z);
        
        if (s->excludes[blocktype]) {
          continue;
        }
        
        Color *bc = mc::MaterialColor[blocktype];
        base->underlay(bc);
        
        if (base->a == 0xff) {
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
          base->overlay(&height);
          break;
      }
      
      img->set_pixel(x, y, base);
    }
  }
  
  return img;
}

Image *Level::get_oblique_image(settings_t *s) {
  Image *img = new Image(mc::MapX, mc::MapY + mc::MapZ);
  
  if (!islevel) {
    return img;
  }

  int blocktype;

  Color light(0, 0, 0, 64);
  
  for (int y = 0; y < mc::MapY; y++) {
    for (int x = 0; x < mc::MapX; x++) {
      for (int z = s->bottom; z < s->top; z++) {
        blocktype = bget(blocks, x, y, z);
        
        if (s->excludes[blocktype]) {
          continue;
        }
        
        Color *bc = mc::MaterialColor[blocktype];
        
        Color height(0, 0, 0, 0);
        height.a = (127 - z);
        Color p(bc);
        p.overlay(&height);
        img->set_pixel(x, y + (mc::MapZ - z) - 1, &p);
        
        Color *bcs = mc::MaterialSideColor[blocktype];
        img->set_pixel(x, y + (mc::MapZ - z), bcs);
      }
    }
  }
  
  return img;
}

Image *Level::get_obliqueangle_image(settings_t *s) {
  Image *img = new Image(mc::MapX * 2 + 1, mc::MapX + mc::MapZ + mc::MapY);
  int blocktype;
  
  if (!islevel) {
    return img;
  }
  
  for (int y = 0; y < mc::MapY; y++) {
    for (int z = s->bottom; z < s->top; z++) {
      for (int x = 0; x < mc::MapX; x++) {
        blocktype = bget(blocks, x, y, z);
    
        if (s->excludes[blocktype]) {
          continue;
        }
    
        Color *bc = mc::MaterialColor[blocktype];
        
        Color height(0, 0, 0, 0);
        height.a = (127 - z);
        Color p(bc);
        p.overlay(&height);
        
        int _x = mc::MapX + x - y;
        int _y = mc::MapZ + x - z + y;
        
        img->set_pixel(_x, _y - 1, &p);
        img->set_pixel(_x + 1, _y - 1, &p);
        
        Color *bcs = mc::MaterialSideColor[blocktype];
        img->set_pixel(_x, _y, bcs);
        img->set_pixel(_x + 1, _y, bcs);
      }
    }
  }
  
  return img;
}

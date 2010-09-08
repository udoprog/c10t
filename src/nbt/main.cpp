#include <iostream>

#include "nbt.h"

nbt::Byte *blocks;
nbt::Byte *skylight;
nbt::Byte *heightmap;
nbt::Byte *blocklight;
nbt::Byte *data;
nbt::Int data_length;
nbt::Int xPos;
nbt::Int zPos;

void register_int(nbt::String name, nbt::Int i) {
  if (name.compare("xPos") == 0) {
    xPos = i;
    return;
  }
  
  if (name.compare("zPos") == 0) {
    zPos = i;
    return;
  }
}

void register_byte_array(nbt::String name, nbt::Int length, nbt::Byte *a) {
  if (name.compare("Blocks") == 0) {
    blocks = a;
    return;
  }

  if (name.compare("SkyLight") == 0) {
    skylight = a;
    return;
  }

  if (name.compare("HeightMap") == 0) {
    heightmap = a;
    return;
  }

  if (name.compare("BlockLight") == 0) {
    blocklight = a;
    return;
  }
  
  if (name.compare("Data") == 0) {
    data = a;
    data_length = length;
    return;
  }
}

using namespace std;

int main(int argc, char *argv[]) {
  nbt::Parser parser;
  parser.register_byte_array = register_byte_array;
  parser.register_int = register_int;
  parser.parse_file(argv[1]);

  cout << xPos << ":" << zPos << endl;
}

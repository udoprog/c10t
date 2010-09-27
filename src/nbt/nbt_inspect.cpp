#include "nbt/nbt.h"

#include <iostream>

using namespace std;

void begin_compound(void*, nbt::String name) {
  cout << "BEGIN Compound(" << name << ")" << endl;
}

void end_compound(void*, nbt::String name) {
  cout << "END Compound(" << name << ")" << endl;
}

void begin_list(void*, nbt::String name, nbt::Byte type, nbt::Int length) {
  cout << "BEGIN List(" << name << ", 0x" << hex << (int)type << ", " << length << ")" << endl;
}

void end_list(void*, nbt::String name) {
  cout << "END List(" << name << ")" << endl;
}

void register_string(void*, nbt::String name, nbt::String value) {
  cout << "String(" << name << "): " << value << endl;
}

void register_short(void*, nbt::String name, nbt::Short value) {
  cout << "Short(" << name << "): " << value << endl;
}

void register_long(void*, nbt::String name, nbt::Long value) {
  cout << "Long(" << name << "): " << value << endl;
}

void register_int(void*, nbt::String name, nbt::Int value) {
  cout << "Int(" << name << "): " << value << endl;
}

void register_byte_array(void*, nbt::String name, nbt::ByteArray* value) {
  cout << "ByteArray(" << name << "): " << value->length << endl;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    return 1;
  }
  
  nbt::Parser<void> parser(NULL);
  parser.begin_compound = begin_compound;
  parser.end_compound = end_compound;
  parser.begin_list = begin_list;
  parser.end_list = end_list;
  parser.register_string = register_string;
  parser.register_short = register_short;
  parser.register_long = register_long;
  parser.register_int = register_int;
  parser.register_byte_array = register_byte_array;
  parser.parse_file(argv[1]);
}

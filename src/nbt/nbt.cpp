#include "nbt.h"

#include <iostream>

void nbt::default_begin_compound(nbt::String name) {
  //std::cout << "TAG_Compound('" << name << "') BEGIN" << std::endl;
}

void nbt::default_end_compound() {
  //std::cout << "TAG_Compound END" << std::endl;
}

void nbt::default_register_long(nbt::String name, nbt::Long l) {
  //std::cout << "TAG_Long('" << name << "'): " << l << std::endl;
}

void nbt::default_register_short(nbt::String name, nbt::Short s) {
  //std::cout << "TAG_Short('" << name << "'): " << (int)s << std::endl;
}

void nbt::default_register_string(nbt::String name, nbt::String s) {
  //std::cout << "TAG_String('" << name << "'): " << s << std::endl;
}

void nbt::default_register_float(nbt::String name, nbt::Float f) {
  //std::cout << "TAG_Float('" << name << "'): " << f << std::endl;
}

void nbt::default_register_double(nbt::String name, nbt::Double f) {
  //std::cout << "TAG_Double('" << name << "'): " << f << std::endl;
}

void nbt::default_register_int(nbt::String name, nbt::Int f) {
  //std::cout << "TAG_Int('" << name << "'): " << f << std::endl;
}

void nbt::default_register_byte(nbt::String name, nbt::Byte f) {
  //std::cout << "TAG_Byte('" << name << "'): " << (int)f << std::endl;
}

void nbt::default_register_byte_array(nbt::String name, nbt::Int length, nbt::Byte *a) {
  //std::cout << "TAG_Byte_Array('" << name << "'): " << length << std::endl;
  //for (int i = 0; i < length && i < 10; i++) {
  //  std::cout << "   " << (int)a[i] << std::endl;
  //}
}

void nbt::default_begin_list(nbt::String name, nbt::Byte type, nbt::Int length) {
  //std::cout << "TAG_List('" << name << "'): " << length << " items" << std::endl;
}

void nbt::default_end_list() {
  //std::cout << "TAG_List END" << std::endl;
}

bool nbt::is_big_endian() {
  int32_t i = 1;
  return ((int8_t*)(&i))[0] == 0;
}
      
void nbt::Parser::handle_type(Byte type, String name, gzFile file)
{
  switch(type) {
  case TAG_Long:    register_long(    name, LongTag::read(file));   break;
  case TAG_Short:   register_short(   name, ShortTag::read(file));  break;
  case TAG_String:  register_string(  name, StringTag::read(file)); break;
  case TAG_Float:   register_float(   name, FloatTag::read(file));  break;
  case TAG_Double:  register_double(  name, DoubleTag::read(file)); break;
  case TAG_Int:     register_int(     name, IntTag::read(file));    break;
  case TAG_Byte:    register_byte(    name, ByteTag::read(file));   break;
  case TAG_Compound:
    handle_compound(name, file);
    break;
  case TAG_Byte_Array:
    handle_byte_array(name, file);
    break;
  case TAG_List:
    handle_list(name, file);
    break;
  }
}

void nbt::Parser::parse_file(const char *path)
{
  gzFile file = gzopen(path, "rb");
  assert(file != NULL);
  Byte type = read_tagType(file);
  
  switch(type) {
  case TAG_Compound:
    String name = StringTag::read(file);
    handle_type(type, name, file);
    break;
  }
  
  assert(file != NULL);
  gzflush(file);
  gzclose(file);
}

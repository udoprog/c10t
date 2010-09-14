#include "nbt.h"

#include <iostream>
#include <stdlib.h>
#include <errno.h>

void nbt::default_begin_compound(void *context, nbt::String name) {
  //std::cout << "TAG_Compound('" << name << "') BEGIN" << std::endl;
}

void nbt::default_end_compound(void *context) {
  //std::cout << "TAG_Compound END" << std::endl;
}

void nbt::default_register_long(void *context, nbt::String name, nbt::Long l) {
  //std::cout << "TAG_Long('" << name << "'): " << l << std::endl;
}

void nbt::default_register_short(void *context, nbt::String name, nbt::Short s) {
  //std::cout << "TAG_Short('" << name << "'): " << (int)s << std::endl;
}

void nbt::default_register_string(void *context, nbt::String name, nbt::String s) {
  //std::cout << "TAG_String('" << name << "'): " << s << std::endl;
}

void nbt::default_register_float(void *context, nbt::String name, nbt::Float f) {
  //std::cout << "TAG_Float('" << name << "'): " << f << std::endl;
}

void nbt::default_register_double(void *context, nbt::String name, nbt::Double f) {
  //std::cout << "TAG_Double('" << name << "'): " << f << std::endl;
}

void nbt::default_register_int(void *context, nbt::String name, nbt::Int f) {
  //std::cout << "TAG_Int('" << name << "'): " << f << std::endl;
}

void nbt::default_register_byte(void *context, nbt::String name, nbt::Byte f) {
  //std::cout << "TAG_Byte('" << name << "'): " << (int)f << std::endl;
}

void nbt::default_register_byte_array(void *context, nbt::String name, nbt::ByteArray *byte_array) {
  //std::cout << "TAG_Byte_Array('" << name << "'): " << length << std::endl;
  //for (int i = 0; i < length && i < 10; i++) {
  //  std::cout << "   " << (int)a[i] << std::endl;
  //}
}

void nbt::default_begin_list(void *context, nbt::String name, nbt::Byte type, nbt::Int length) {
  //std::cout << "TAG_List('" << name << "'): " << length << " items" << std::endl;
}

void nbt::default_end_list(void *context) {
  //std::cout << "TAG_List END" << std::endl;
}

void nbt::default_error_handler(void *context, size_t where, const char *why) {
  std::cerr << "Unhandled nbt parser error at byte " << where << ": " << why << std::endl;
  exit(1);
}

bool nbt::is_big_endian() {
  int32_t i = 1;
  return ((int8_t*)(&i))[0] == 0;
}
      
void nbt::Parser::handle_type(Byte type, String name, gzFile file)
{
  switch(type) {
  case TAG_Long:    register_long(context,    name, read_long(file));   break;
  case TAG_Short:   register_short(context,   name, read_short(file));  break;
  case TAG_String:  register_string(context,  name, read_string(file)); break;
  case TAG_Float:   register_float(context,   name, read_float(file));  break;
  case TAG_Double:  register_double(context,  name, read_double(file)); break;
  case TAG_Int:     register_int(context,     name, read_int(file));    break;
  case TAG_Byte:    register_byte(context,    name, read_byte(file));   break;
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
  assert_error(file, file != NULL, strerror(errno));
  
  Byte type = read_tagType(file);
  
  switch(type) {
  case TAG_Compound:
    String name = read_string(file);
    handle_type(type, name, file);
    break;
  }
  
  gzclose(file);
}

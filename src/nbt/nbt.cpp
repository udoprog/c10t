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
  case TAG_Long:
    if (register_long == NULL) {
      assert_error(file, gzseek(file, sizeof(nbt::Long), SEEK_CUR) != -1,
        "Buffer too short to flush long");
    } else {
      register_long(context, name, read_long(file));
    }
    break;
  case TAG_Short:
    if (register_short == NULL) {
      assert_error(file, gzseek(file, sizeof(nbt::Short), SEEK_CUR) != -1,
        "Buffer too short to flush short");
    } else {
      register_short(context, name, read_short(file));
    }
    break;
  case TAG_String:
    if (register_short == NULL) {
      flush_string(file);
    } else {
      register_string(context, name, read_string(file));
    }
    break;
  case TAG_Float:
    if (register_float == NULL) {
      assert_error(file, gzseek(file, sizeof(nbt::Float), SEEK_CUR) != -1,
        "Buffer too short to flush float");
    } else {
      register_float(context, name, read_float(file));
    }
    break;
  case TAG_Double:
    if (register_double == NULL) {
      assert_error(file, gzseek(file, sizeof(nbt::Double), SEEK_CUR) != -1,
        "Buffer too short to flush double");
    } else {
      register_double(context, name, read_double(file));
    }
    break;
  case TAG_Int:
    if (register_int == NULL) {
      assert_error(file, gzseek(file, sizeof(nbt::Int), SEEK_CUR) != -1,
        "Buffer too short to flush int");
    } else {
      register_int(context, name, read_int(file));
    }
    break;
  case TAG_Byte:
    if (register_byte == NULL) {
      assert_error(file, gzseek(file, sizeof(nbt::Byte), SEEK_CUR) != -1,
        "Buffer too short to flush byte");
    } else {
      register_byte(context, name, read_byte(file));
    }
    break;
  case TAG_Compound:
    handle_compound(name, file);
    break;
  case TAG_Byte_Array:
    if (register_byte_array == NULL) {
      flush_byte_array(file);
    } else {
      handle_byte_array(name, file);
    }
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

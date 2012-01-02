#ifndef _NBT_TYPES_HPP
#define _NBT_TYPES_HPP

#include <stdint.h>
#include <string>

namespace nbt {
  typedef int8_t Byte;
  typedef int16_t Short;
  typedef int32_t Int;
  typedef int64_t Long;
  typedef std::string String;
  typedef float Float;
  typedef double Double;
  
  struct ByteArray {
    Int length;
    Byte *values;
    ~ByteArray() {
      delete [] values;
    }
  };

  struct stack_entry {
    Byte type;
    String name;
    Int list_count, list_read;
    Byte list_type;
  };
}

#endif /* _NBT_TYPES_HPP */

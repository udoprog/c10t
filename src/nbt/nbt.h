#ifndef _NBT_H_
#define _NBT_H_

#include <string>
#include <stdint.h>
#include <zlib.h>
#include <assert.h>
#include <stack>
#include <list>

#include <iostream>

namespace nbt {
  #define BUFFER_SIZE 1024
  
  typedef int8_t Byte;
  typedef int16_t Short;
  typedef int32_t Int;
  typedef int64_t Long;
  typedef std::string String;
  typedef float Float;
  typedef double Double;
  
  const Byte TAG_End = 0x0;
  const Byte TAG_Byte = 0x1;
  const Byte TAG_Short = 0x2;
  const Byte TAG_Int = 0x3;
  const Byte TAG_Long = 0x4;
  const Byte TAG_Float = 0x5;
  const Byte TAG_Double = 0x6;
  const Byte TAG_Byte_Array = 0x7;
  const Byte TAG_String = 0x8;
  const Byte TAG_List = 0x9;
  const Byte TAG_Compound = 0xa;

  const std::string TAG_End_str("TAG_End");
  const std::string TAG_Byte_str("TAG_Byte");
  const std::string TAG_Short_str("TAG_Short");
  const std::string TAG_Int_str("TAG_Int");
  const std::string TAG_Long_str("TAG_Long");
  const std::string TAG_Float_str("TAG_Float");
  const std::string TAG_Double_str("TAG_Double");
  const std::string TAG_Byte_Array_str("TAG_Byte_Array");
  const std::string TAG_String_str("TAG_String");
  const std::string TAG_List_str("TAG_List");
  const std::string TAG_Compound_str("TAG_Compound");
  
  const std::string tag_string_map[] = {
    TAG_End_str,
    TAG_Byte_str,
    TAG_Short_str,
    TAG_Int_str,
    TAG_Long_str,
    TAG_Float_str,
    TAG_Double_str,
    TAG_Byte_Array_str,
    TAG_String_str,
    TAG_List_str,
    TAG_Compound_str
  };
  
  typedef void (*begin_compound_t)(String name);
  typedef void (*end_compound_t)();
  
  typedef void (*begin_list_t)(String name, Byte type, Int length);
  typedef void (*end_list_t)();
  
  typedef void (*register_long_t)(String name, Long l);
  typedef void (*register_short_t)(String name, Short l);
  typedef void (*register_string_t)(String name, String l);
  typedef void (*register_float_t)(String name, Float l);
  typedef void (*register_double_t)(String name, Double l);
  typedef void (*register_int_t)(String name, Int l);
  typedef void (*register_byte_t)(String name, Byte b);
  typedef void (*register_byte_array_t)(String name, Int length, Byte *b);

  void default_begin_compound(nbt::String name);
  void default_end_compound();
  void default_register_long(nbt::String name, nbt::Long l);
  void default_register_short(nbt::String name, nbt::Short s);
  void default_register_string(nbt::String name, nbt::String s);
  void default_register_float(nbt::String name, nbt::Float f);
  void default_register_double(nbt::String name, nbt::Double f);
  void default_register_int(nbt::String name, nbt::Int f);
  void default_register_byte(nbt::String name, nbt::Byte f);
  void default_register_byte_array(nbt::String name, nbt::Int length, nbt::Byte *a);
  void default_begin_list(nbt::String name, nbt::Byte type, nbt::Int length);
  void default_end_list();
  
  bool is_big_endian();
  
  class Tag {
    public:
  };

  class ShortTag : Tag {
    public:
      static Short read(gzFile file) {
        uint8_t b[2];
        assert(gzread(file, b, sizeof(b)) != -1);
        Short s = (b[0] << 8) + b[1];
        return s;
      }
  };

  class StringTag : Tag {
    public:
      static String read(gzFile file) {
        Short s = ShortTag::read(file);
        uint8_t str[s + 1];
        assert(gzread(file, str, (size_t)s) != -1);
        return String((const char*)str, s);
      }
  };
  
  class FloatTag : Tag {
    public:
      static Float read(gzFile file)
      {
        Float f;
        Byte b[sizeof(f)];
        assert(gzread(file, b, sizeof(f)) != -1);
        Float *fp = &f;
        
        if (is_big_endian()) {
          *((Byte*)fp) = b[0];
          *((Byte*)fp + 1) = b[1];
          *((Byte*)fp + 2) = b[2];
          *((Byte*)fp + 3) = b[3];
        } else {
          *((Byte*)fp) = b[3];
          *((Byte*)fp + 1) = b[2];
          *((Byte*)fp + 2) = b[1];
          *((Byte*)fp + 3) = b[0];
        }
        
        return f;
      }
  };
      
  class LongTag : Tag {
    public:
      static Long read(gzFile file) {
        Long l;
        Byte b[sizeof(l)];
        assert(gzread(file, b, sizeof(b)) != -1);
        Long *lp = &l;
        
        if (is_big_endian()) {
          *((Byte*)lp) = b[0];
          *((Byte*)lp + 1) = b[1];
          *((Byte*)lp + 2) = b[2];
          *((Byte*)lp + 3) = b[3];
          *((Byte*)lp + 4) = b[4];
          *((Byte*)lp + 5) = b[5];
          *((Byte*)lp + 6) = b[6];
          *((Byte*)lp + 7) = b[7];
        } else {
          *((Byte*)lp) = b[7];
          *((Byte*)lp + 1) = b[6];
          *((Byte*)lp + 2) = b[5];
          *((Byte*)lp + 3) = b[4];
          *((Byte*)lp + 4) = b[3];
          *((Byte*)lp + 5) = b[2];
          *((Byte*)lp + 6) = b[1];
          *((Byte*)lp + 7) = b[0];
        }
        
        return l;
      }
  };

  class DoubleTag : Tag {
    public:
      static Double read(gzFile file) {
        Double d;
        Byte b[sizeof(d)];
        assert(gzread(file, b, sizeof(d)) != -1);
        Double *dp = &d;
        
        if (is_big_endian()) {
          *((Byte*)dp) = b[0];
          *((Byte*)dp + 1) = b[1];
          *((Byte*)dp + 2) = b[2];
          *((Byte*)dp + 3) = b[3];
          *((Byte*)dp + 4) = b[4];
          *((Byte*)dp + 5) = b[5];
          *((Byte*)dp + 6) = b[6];
          *((Byte*)dp + 7) = b[7];
        } else {
          *((Byte*)dp) = b[7];
          *((Byte*)dp + 1) = b[6];
          *((Byte*)dp + 2) = b[5];
          *((Byte*)dp + 3) = b[4];
          *((Byte*)dp + 4) = b[3];
          *((Byte*)dp + 5) = b[2];
          *((Byte*)dp + 6) = b[1];
          *((Byte*)dp + 7) = b[0];
        }
        
        return d;
      }
  };

  class IntTag : Tag {
    public:
      static Int read(gzFile file) {
        Int i;
        Byte b[sizeof(i)];
        assert(gzread(file, b, sizeof(b)) != -1);
        Int *ip = &i;
        
        if (is_big_endian()) {
          *((Byte*)ip) = b[0];
          *((Byte*)ip + 1) = b[1];
          *((Byte*)ip + 2) = b[2];
          *((Byte*)ip + 3) = b[3];
        } else {
          *((Byte*)ip) = b[3];
          *((Byte*)ip + 1) = b[2];
          *((Byte*)ip + 2) = b[1];
          *((Byte*)ip + 3) = b[0];
        }
        
        return i;
      }
  };

  class ByteTag : Tag {
    public:
      static Byte read(gzFile file) {
        Byte b;
        assert(gzread(file, &b, sizeof(Byte)) != -1);
        return b;
      }
  };
  
  class Parser {
    private:
      std::stack<Byte> context;
    public:
      begin_compound_t begin_compound;
      register_long_t register_long;
      register_short_t register_short;
      register_string_t register_string;
      register_float_t register_float;
      register_double_t register_double;
      register_int_t register_int;
      register_byte_t register_byte;
      register_byte_array_t register_byte_array;
      end_compound_t end_compound;
      
      begin_list_t begin_list;
      end_list_t end_list;
      
      Parser() :
        context(),
        begin_compound(default_begin_compound),
        register_long(default_register_long),
        register_short(default_register_short),
        register_string(default_register_string),
        register_float(default_register_float),
        register_double(default_register_double),
        register_int(default_register_int),
        register_byte(default_register_byte),
        register_byte_array(default_register_byte_array),
        end_compound(default_end_compound),
        begin_list(default_begin_list),
        end_list(default_end_list)
      {
      }
      
      Byte read_tagType(gzFile file) {
        Byte type = ByteTag::read(file);
        assert(type >= 0 && type <= TAG_Compound);
        return type;
      }
      
      void handle_list(String name, gzFile file) {
        Byte type = ByteTag::read(file);
        Int length = IntTag::read(file);
        
        begin_list(name, type, length);
        
        for (int i = 0; i < length; i++) {
          handle_type(type, name, file);
        }

        end_list();
      }
      
      void handle_byte_array(String name, gzFile file) {
        Int length = IntTag::read(file);
        Byte *a = new Byte[length];
        assert(gzread(file, a, length) != -1);
        register_byte_array(name, length, a);
      }

      void handle_compound(String name, gzFile file) {
        begin_compound(name);
        
        do {
          Byte type = read_tagType(file);
          
          if (type == TAG_End) {
            break;
          }
          
          name = StringTag::read(file);
          handle_type(type, name, file);
        } while(1);
        
        end_compound();
      }
      
      void handle_type(Byte type, String name, gzFile file)
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
      
      void parse_file(const char *path) {
        gzFile file = gzopen(path, "rb");
        
        Byte type = read_tagType(file);
        
        switch(type) {
        case TAG_Compound:
          String name = StringTag::read(file);
          handle_type(type, name, file);
          break;
        }
      }
  };
  
  class NBTFile {
    private:
      std::list<Tag> tags;
    public:
      NBTFile() : tags() {
      }
  };
};

#endif /* _NBT_H_ */

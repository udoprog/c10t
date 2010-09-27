#ifndef _NBT_H_
#define _NBT_H_

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <zlib.h>

#include <iostream>
#include <string.h>
#include <exception>
#include <string>
#include <stack>
#include <list>

#include <boost/ptr_container/ptr_vector.hpp>

namespace nbt {
  class bad_grammar : std::exception {};
  #define NBT_STACK_SIZE 100
  
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
  
  bool is_big_endian();
  
  template <class C>
  void default_begin_compound(C* context, nbt::String name) {
    //std::cout << "TAG_Compound('" << name << "') BEGIN" << std::endl;
  }

  template <class C>
  void default_end_compound(C* context, String name) {
    //std::cout << "TAG_Compound END" << std::endl;
  }

  template <class C>
  void default_begin_list(C* context, nbt::String name, nbt::Byte type, nbt::Int length) {
    //std::cout << "TAG_List('" << name << "'): " << length << " items" << std::endl;
  }

  template <class C>
  void default_end_list(C* context, nbt::String name) {
    //std::cout << "TAG_List END" << std::endl;
  }

  template <class C>
  void default_error_handler(C* context, size_t where, const char *why) {
    std::cerr << "Unhandled nbt parser error at byte " << where << ": " << why << std::endl;
    exit(1);
  }
  
  template <class C>
  class Parser {
    private:
      bool running;
      C *context;

      void assert_error(gzFile file, bool a, const char *why) {
        if (!a) {
          size_t where = file == NULL ? 0 : gztell(file);
          error_handler(context, where, why);
          stop();
        }
      }

      Byte read_byte(gzFile file) {
        Byte b;
        assert_error(file, gzread(file, &b, sizeof(Byte)) == sizeof(Byte), "Buffer too short to read Byte");
        return b;
      }
      
      Short read_short(gzFile file) {
        uint8_t b[2];
        assert_error(file, gzread(file, b, sizeof(b)) == sizeof(b), "Buffer to short to read Short");
        Short s = (b[0] << 8) + b[1];
        return s;
      }

      Int read_int(gzFile file) {
        Int i;
        Byte b[sizeof(i)];
        assert_error(file, gzread(file, b, sizeof(b)) == sizeof(b), "Buffer to short to read Int");
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
      
      String read_string(gzFile file) {
        Short s = read_short(file);
        uint8_t *str = new uint8_t[s + 1];
        assert_error(file, gzread(file, str, s) == s, "Buffer to short to read String");
        String so((const char*)str, s);
        delete [] str;
        return so;
      }
      
      void flush_string(gzFile file) {
        Short s = read_short(file);
        assert_error(file, gzseek(file, s, SEEK_CUR) != -1, "Buffer to short to flush String");
      }
      
      Float read_float(gzFile file)
      {
        Float f;
        Byte b[sizeof(f)];
        assert_error(file, gzread(file, b, sizeof(f)) == sizeof(f), "Buffer to short to read Float");
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
      
      Long read_long(gzFile file) {
        Long l;
        Byte b[sizeof(l)];
        assert_error(file, gzread(file, b, sizeof(b)) == sizeof(b), "Buffer to short to read Long");
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
      
      Double read_double(gzFile file) {
        Double d;
        Byte b[sizeof(d)];
        assert_error(file, gzread(file, b, sizeof(d)) == sizeof(d), "Buffer to short to read Double");
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
    public:
      typedef void (*begin_compound_t)(C*, String name);
      typedef void (*end_compound_t)(C*, String name);
      
      typedef void (*begin_list_t)(C*, String name, Byte type, Int length);
      typedef void (*end_list_t)(C*, String name);
      
      typedef void (*register_long_t)(C*, String name, Long l);
      typedef void (*register_short_t)(C*, String name, Short l);
      typedef void (*register_string_t)(C*, String name, String l);
      typedef void (*register_float_t)(C*, String name, Float l);
      typedef void (*register_double_t)(C*, String name, Double l);
      typedef void (*register_int_t)(C*, String name, Int l);
      typedef void (*register_byte_t)(C*, String name, Byte b);
      typedef void (*register_byte_array_t)(C*, String name, ByteArray* array);
      typedef void (*error_handler_t)(C*, size_t where, const char *why);
      
      register_long_t register_long;
      register_short_t register_short;
      register_string_t register_string;
      register_float_t register_float;
      register_double_t register_double;
      register_int_t register_int;
      register_byte_t register_byte;
      register_byte_array_t register_byte_array;

      begin_compound_t begin_compound;
      end_compound_t end_compound;
      begin_list_t begin_list;
      end_list_t end_list;
      error_handler_t error_handler;
      
      Parser() :
        context(NULL),
        register_long(NULL),
        register_short(NULL),
        register_string(NULL),
        register_float(NULL),
        register_double(NULL),
        register_int(NULL),
        register_byte(NULL),
        register_byte_array(NULL),
        begin_compound(&default_begin_compound<C>),
        end_compound(&default_end_compound<C>),
        begin_list(&default_begin_list<C>),
        end_list(&default_end_list<C>),
        error_handler(&default_error_handler<C>)
      {
      }
      
      Parser(C *context) :
        context(context),
        register_long(NULL),
        register_short(NULL),
        register_string(NULL),
        register_float(NULL),
        register_double(NULL),
        register_int(NULL),
        register_byte(NULL),
        register_byte_array(NULL),
        begin_compound(&default_begin_compound<C>),
        end_compound(&default_end_compound<C>),
        begin_list(&default_begin_list<C>),
        end_list(&default_end_list<C>),
        error_handler(&default_error_handler<C>)
      {
        this->context = context;
      }
      
      Byte read_tagType(gzFile file) {
        Byte type = read_byte(file);
        assert_error(file, type >= 0 && type <= TAG_Compound, "Not a valid tag type");
        return type;
      }
      
      void flush_byte_array(gzFile file) {
        Int length = read_int(file);
        assert_error(file, gzseek(file, length, SEEK_CUR) != -1,
          "Buffer to short to flush ByteArray");
      }
      
      void handle_byte_array(String name, gzFile file) {
        Int length = read_int(file);
        Byte *values = new Byte[length];
        assert_error(file, gzread(file, values, length) == length, "Buffer to short to read ByteArray");
        ByteArray *array = new ByteArray();
        array->values = values;
        array->length = length;
        register_byte_array(context, name, array);
      }

      void stop() {
        running = false;
      }
      
      void parse_file(const char *path)
      {
        gzFile file = gzopen(path, "rb");
        assert_error(file, file != NULL, strerror(errno));

        running = true;
        
        stack_entry *stack = new stack_entry[NBT_STACK_SIZE];
        int stack_p = 0;
        
        stack_entry *root = stack + 0;
        
        root->type = read_tagType(file);
        assert_error(file, root->type == TAG_Compound, "Expected TAG_Compound at root");
        root->name = read_string(file);
        
        begin_compound(context, root->name);
        
        while(running && stack_p >= 0) {
          assert_error(file, stack_p < NBT_STACK_SIZE, "Stack cannot be larger than NBT_STACK_SIZE");
          
          stack_entry* top = stack + stack_p;
          
          Byte type;
          String name;
          
          if (top->type == TAG_Compound) {
            type = read_tagType(file);
            
            if (type == TAG_End) {
              end_compound(context, top->name);
              --stack_p;
              continue;
            }
            
            name = read_string(file);
          }
          
          else if (top->type == TAG_List) {
            if (top->list_read >= top->list_count) {
              end_list(context, top->name);
              --stack_p;
              continue;
            }
            
            name = top->name;
            type = top->list_type;
            top->list_read++;
          }
          
          else {
            assert_error(file, 1, "Unknown stack type");
            continue;
          }
          
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
            if (register_string == NULL) {
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
          case TAG_List:
            {
              stack_entry* c = stack + ++stack_p;
              
              c->list_read = 0;
              c->list_type = read_tagType(file);
              c->list_count = read_int(file);
              c->name = name;
              c->type = TAG_List;
              
              begin_list(context, name, c->list_type, c->list_count);
            }
            break;
          case TAG_Compound:
            {
              begin_compound(context, name);
              stack_entry* c = stack + ++stack_p;
              
              c->list_read = 0;
              c->list_count = 0;
              c->list_type = -1;
              c->name = name;
              c->type = TAG_Compound;
            }
            break;
          case TAG_Byte_Array:
            if (register_byte_array == NULL) {
              flush_byte_array(file);
            } else {
              handle_byte_array(name, file);
            }
            break;
          default:
            assert_error(file, 0, "Encountered unknown type");
            break;
          }
        }
        
        gzclose(file);
      }
  };
}

#endif /* _NBT_H_ */

#include <string>
#include <stdint.h>
#include <zlib.h>
#include <assert.h>
#include <stack>

#include <stdio.h>

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
  typedef void (*add_compound_long_t)(String name, Long l);
  typedef void (*add_compound_short_t)(String name, Short l);
  typedef void (*add_compound_string_t)(String name, String l);
  typedef void (*add_compound_float_t)(String name, Float l);
  typedef void (*add_compound_int_t)(String name, Int l);
  typedef void (*add_compound_byte_t)(String name, Byte b);
  typedef void (*end_compound_t)();
  
  typedef void (*begin_list_t)(String name, Byte type, Int length);
  typedef void (*end_list_t)();

  class Parser {
    private:
      std::stack<Byte> context;
    public:
      begin_compound_t begin_compound;
      add_compound_long_t add_compound_long;
      add_compound_short_t add_compound_short;
      add_compound_string_t add_compound_string;
      add_compound_float_t add_compound_float;
      add_compound_int_t add_compound_int;
      add_compound_byte_t add_compound_byte;
      end_compound_t end_compound;
      
      begin_list_t begin_list;
      end_list_t end_list;
      
      Parser() :
        context(),
        begin_compound(NULL),
        add_compound_long(NULL),
        add_compound_short(NULL),
        add_compound_string(NULL),
        add_compound_float(NULL),
        add_compound_int(NULL),
        add_compound_byte(NULL),
        end_compound(NULL),
        begin_list(NULL),
        end_list(NULL)
      {
      }
      
      Byte read_tagType(gzFile file) {
        Byte type = read_byte(file);
        assert(type >= 0 && type <= TAG_Compound);
        return type;
      }
      
      Byte read_byte(gzFile file) {
        Byte b;
        assert(gzread(file, &b, sizeof(Byte)) != -1);
        return b;
      }
      
      Short read_short(gzFile file) {
        uint8_t b[2];
        assert(gzread(file, b, sizeof(b)) != -1);
        Short s =
          (b[0] << 8) +
          b[1];
        return s;
      }
      
      Int read_int(gzFile file) {
        uint8_t b[4];
        assert(gzread(file, b, sizeof(b)) != -1);
        
        Int i =
          ((Int)b[0] << 24) +
          ((Int)b[1] << 16) +
          ((Int)b[2] << 8) +
          ((Int)b[3]);
        
        return i;
      }

      Float read_float(gzFile file) {
        Float f;
        assert(gzread(file, &f, sizeof(f)) != -1);
        return f;
      }
      
      Double read_double(gzFile file) {
        Double d;
        assert(gzread(file, &d, sizeof(d)) != -1);
        return d;
      }

      Long read_long(gzFile file) {
        uint8_t b[8];
        assert(gzread(file, b, sizeof(b)) != -1);

        Long l = 
          ((Long)b[0] << 56) +
          ((Long)b[1] << 48) +
          ((Long)b[2] << 40) +
          ((Long)b[3] << 32) +
          ((Long)b[4] << 24) +
          ((Long)b[5] << 16) +
          ((Long)b[6] << 8) +
          (Long)b[7];
        
        return l;
      }
      
      String read_string(gzFile file) {
        Short s = read_short(file);
        uint8_t str[s + 1];
        assert(gzread(file, str, (size_t)s) != -1);
        return String((const char*)str, s);
      }

      void handle_list(String name, gzFile file) {
        Byte type = read_byte(file);
        Int length = read_int(file);
        
        begin_list(name, type, length);
        
        for (int i = 0; i < length; i++) {
          handle_type(type, name, file);
        }
      }

      void handle_type(Byte type, String name, gzFile file) {
        std::cout << "T: " << tag_string_map[type] << std::endl;
        
        switch(type) {
        case TAG_Long:
          add_compound_long(name, read_long(file));
          break;
        case TAG_Short:
          add_compound_short(name, read_short(file));
          break;
        case TAG_String:
          add_compound_string(name, read_string(file));
          break;
        case TAG_Float:
          add_compound_float(name, read_float(file));
          break;
        case TAG_Int:
          add_compound_int(name, read_int(file));
          break;
        case TAG_Byte:
          add_compound_byte(name, read_byte(file));
          break;
        case TAG_Compound:
          if (begin_compound != NULL) begin_compound(name);
          handle_compound(file);
          break;
        case TAG_List:
          handle_list(name, file);
          break;
        }
      }
      
      void handle_compound(gzFile file) {
        String name;
        
        do {
          Byte type = read_tagType(file);
          
          if (type == TAG_End) {
            break;
          }
          
          name = read_string(file);
          handle_type(type, name, file);
        } while(1);

        if (end_compound != NULL) end_compound();
      }
      
      void parse_file(const char *path) {
        gzFile file = gzopen(path, "rb");
        
        do {
          Byte type = read_tagType(file);
          std::cout << tag_string_map[type] << std::endl;
          
          switch(type) {
          case TAG_Compound:
            String name = read_string(file);
            if (begin_compound != NULL) begin_compound(name);
            handle_compound(file);
            break;
          }
        } while(!context.empty());
      }
  };
};

using namespace std;

#include <iostream>

void begin_compound(nbt::String name) {
  cout << "TAG Compound: " << name << endl;
}

void end_compound() {
  cout << "END" << endl;
}

void add_compound_long(nbt::String name, nbt::Long l) {
  cout << "TAG_Long('" << name << "'): " << l << endl;
}

void add_compound_short(nbt::String name, nbt::Short s) {
  cout << "TAG_Short('" << name << "'): " << (int)s << endl;
}

void add_compound_string(nbt::String name, nbt::String s) {
  cout << "TAG_String('" << name << "'): " << s << endl;
}

void add_compound_float(nbt::String name, nbt::Float f) {
  cout << "TAG_Float('" << name << "'): " << f << endl;
}

void add_compound_int(nbt::String name, nbt::Int f) {
  cout << "TAG_Int('" << name << "'): " << f << endl;
}

void add_compound_byte(nbt::String name, nbt::Byte f) {
  cout << "TAG_Byte('" << name << "'): " << (int)f << endl;
}

void begin_list(nbt::String name, nbt::Byte type, nbt::Int length) {
  cout << "TAG_List('" << name << "'): " << length << " items" << endl;
}

int main(int argc, char *argv[]) {
  nbt::Parser parser;
  parser.begin_compound = begin_compound;
  parser.add_compound_long = add_compound_long;
  parser.add_compound_short = add_compound_short;
  parser.add_compound_string = add_compound_string;
  parser.add_compound_float = add_compound_float;
  parser.add_compound_int = add_compound_int;
  parser.add_compound_byte = add_compound_byte;
  parser.end_compound = end_compound;
  parser.begin_list = begin_list;
  parser.parse_file(argv[1]);
}

#ifndef _JSON_H_
#define _JSON_H_

#include <map>
#include <string>
#include <vector>

#include <ostream>

#include "utf8.h"

namespace json {
  enum object_type {
    Null,
    Int,
    String
  };
  
  static void encode_string(std::ostream& out, std::string s) {
    std::vector<uint32_t> str = utf8_decode(s);

    std::vector<uint32_t>::iterator it;
    
    out << "\"";

    for (it = str.begin(); it != str.end(); it++) {
      uint32_t c = *it;
      
      switch(c) {
        case '"': out << "\\\""; break;
        case '\\': out << "\\\\"; break;
        case '/': out << "\\/"; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
          utf8_encode(c, out);
          break;
      }
    }

    out << "\"";
  }
  
  class value {
    private:
      int int_value;
      std::string string_value;
    public:
      object_type type;
      
      value() : type(Null) {
      }
      
      value(int v) : int_value(v), type(Int) {
      }
      
      value(std::string v) : string_value(v), type(String) {
      }
      
      friend std::ostream& operator<<(std::ostream& out, const value& o) {
        switch(o.type) {
          case Int: out << o.int_value; break;
          case String: encode_string(out, o.string_value); break;
          case Null: out << "null"; break;
        }
        
        return out;
      }
  };
  
  class object {
    private:
      std::map<std::string, value> a;
    public:
      value& operator[] (const std::string s) {
        return a[s];
      }
      
      friend std::ostream& operator<<(std::ostream& out, object& w) {
        out << "{";

        unsigned int i = 0;
        
        std::map<std::string, value>::iterator it;
        
        for (it = w.a.begin(); it != w.a.end(); it++) {
          encode_string(out, (*it).first);
          out << ":";
          out << (*it).second;
          if (++i < w.a.size()) out << ",";
        }
        
        out << "}";
        return out;
      }
  };
  
  class array {
    private:
      std::vector<object> a;
    public:
      void push(object& o) {
        a.push_back(o);
      }
      
      friend std::ostream& operator<<(std::ostream& out, array& a) {
        out << "[";

        unsigned int i = 0;
        
        std::vector<object>::iterator it;
        
        for (it = a.a.begin(); it != a.a.end(); it++) {
          out << *it;
          if (++i < a.a.size()) out << ",";
        }
        
        out << "]";
        return out;
      }
  };
};

#endif /* _JSON_H_ */

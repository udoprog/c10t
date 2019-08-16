#ifndef _JSON_H_
#define _JSON_H_

#include <map>
#include <string>
#include <vector>

#include <ostream>

namespace json {
  enum json_type {
    String,
    Number,
    Object,
    Array,
    None
  };

  class string;
  class number;
  class object;
  class array;
  
  template<typename T>
  struct Trait {
    const static json_type type = None;
  };
  
  template<>
  struct Trait<string> {
    const static enum json_type type = String;
  };
  
  template<>
  struct Trait<number> {
    const static enum json_type type = Number;
  };

  template<>
  struct Trait<object> {
    const static enum json_type type = Object;
  };
  
  template<>
  struct Trait<array> {
    const static enum json_type type = Array;
  };
  
  void encode_string(std::ostream& os, std::string s);

  class basic_json {
    public:
      virtual ~basic_json() {};
      virtual void write(std::ostream& os) = 0;
      virtual json_type get_type() = 0;
  };
  
  class string : public basic_json {
    private:
      std::string s;
    public:
      const static json_type type = Trait<string>::type;
      json_type get_type() { return Trait<string>::type; }
      
      string(std::string s) : s(s) {}
      void write(std::ostream& os) {
        encode_string(os, s);
      }
  };

  class number : public basic_json {
    private:
      int v;
    public:
      const static json_type type = Trait<number>::type;
      json_type get_type() { return Trait<number>::type; }

      number(int v) : v(v) {}
      void write(std::ostream& os) {
        os << v;
      }
  };
  
  class object : public basic_json {
    private:
      std::map<std::string, basic_json*> a;
    public:
      const static json_type type = Trait<object>::type;
      json_type get_type() { return Trait<object>::type; }
      
      void put(std::string k, basic_json* o) {
        a[k] = o;
      }
      
      void write(std::ostream& os) {
        os << "{";

        unsigned int i = 0;
        
        std::map<std::string, basic_json*>::iterator it;
        
        for (it = a.begin(); it != a.end(); it++) {
          encode_string(os, (*it).first);
          os << ":";
          (*it).second->write(os);
          if (++i < a.size()) os << ",";
        }
        
        os << "}";
      }
      
      friend std::ostream& operator<<(std::ostream& os, object& w) {
        w.write(os);
        return os;
      }
      
      ~object() {
        std::map<std::string, basic_json*>::iterator it;
        
        for (it = a.begin(); it != a.end(); it++) {
          delete (*it).second;
        }
      }
  };
  
  class array : public basic_json {
    private:
      std::vector<basic_json*> a;
    public:
      const static json_type type = Trait<array>::type;
      json_type get_type() { return Trait<array>::type; }
      
      void push(basic_json* o) { a.push_back(o); }
      void push(basic_json& o) { a.push_back(&o); }
      
      void write(std::ostream& os) {
        os << "[";

        unsigned int i = 0;
        
        std::vector<basic_json*>::iterator it;
        
        for (it = a.begin(); it != a.end(); it++) {
          (*it)->write(os);
          if (++i < a.size()) os << ",";
        }
        
        os << "]";
      }
      
      friend std::ostream& operator<<(std::ostream& os, array& a) {
        a.write(os);
        return os;
      }

      ~array() {
        std::vector<basic_json*>::iterator it;
        for (it = a.begin(); it != a.end(); it++) {
          delete *it;
        }
      }
  };
  
  template<typename T>
  T* cast(basic_json *b) {
    return reinterpret_cast<T*>(b);
  }
}

#endif /* _JSON_H_ */

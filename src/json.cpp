#include "json.hpp"

#include <unc/unc.hpp>

namespace json {
  void encode_string(std::ostream& os, std::string s) {
    unc::ustring str = unc::decode<unc::utf8>(s);

    unc::ustring::iterator it;
    
    os << "\"";

    for (it = str.begin(); it != str.end(); it++) {
      unc::codepoint_t c = *it;
      
      switch(c) {
        case '"': os << "\\\""; break;
        case '\\': os << "\\\\"; break;
        case '/': os << "\\/"; break;
        case '\b': os << "\\b"; break;
        case '\f': os << "\\f"; break;
        case '\n': os << "\\n"; break;
        case '\r': os << "\\r"; break;
        case '\t': os << "\\t"; break;
        default:
          std::string s;
          unc::encode_codepoint<unc::utf8>(c, s);
          os << s;
          break;
      }
    }

    os << "\"";
  }
}

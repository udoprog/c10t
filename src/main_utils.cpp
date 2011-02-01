#include "main_utils.hpp"

#include <sstream>
#include <fstream>
#include <iomanip>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

stringstream error;

template<typename C>
void boost_split(C& collection, const string& s) {
  char_separator<char> sep(" \t\n\r,:");
  typedef tokenizer<boost::char_separator<char> > tokenizer;
  tokenizer tokens(s, sep);

  for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
    collection.insert(collection.end(), *tok_iter);
  }
}

bool get_blockid(const string blockid_string, int& blockid)
{
  for (int i = 0; i < mc::MaterialCount; i++) {
    if (blockid_string.compare(mc::MaterialName[i]) == 0) {
      blockid = i;
      return true;
    }
  }
  
  try {
    blockid = lexical_cast<int>(blockid_string);
  } catch(const bad_lexical_cast& e) {
    error << "Cannot be converted to number: " << blockid_string;
    return false;
  }
  
  if (!(blockid >= 0 && blockid < mc::MaterialCount)) {
    error << "Not a valid blockid: " << blockid_string;
    return false;
  }
  
  return true;
}

bool parse_color(const string value, color& c)
{
  std::vector<std::string> parts;

  boost_split(parts, value);

  int cr, cg, cb, ca=0xff;
  
  if (parts.size() < 3) {
    error << "Color sets must be of the form <red>,<green>,<blue>[,<alpha>] but was: " << value;
    return false;
  }

  try {
    cr = lexical_cast<int>(parts[0]);
    cg = lexical_cast<int>(parts[1]);
    cb = lexical_cast<int>(parts[2]);
    if (parts.size() == 4) {
      ca = lexical_cast<int>(parts[3]);
    }
  } catch(const bad_lexical_cast& e) {
    error << "Cannot be converted to a color: " << value;
    return false;
  }
  
  if (!(
      cr >= 0 && cr <= 0xff &&
      cg >= 0 && cg <= 0xff &&
      cb >= 0 && cb <= 0xff &&
      ca >= 0 && ca <= 0xff)) {
    error << "Color values must be between 0-255: " << value;
    return false;
  }
  
  c.r = cr;
  c.g = cg;
  c.b = cb;
  c.a = ca;
  
  return true;
}

bool parse_set(const char* set_str, int& blockid, color& c)
{
  istringstream iss(set_str);
  string key, value;
  
  assert(getline(iss, key, '='));
  assert(getline(iss, value));
  
  if (!get_blockid(key, blockid)) {
    return false;
  }

  if (!parse_color(value, c)) {
    return false;
  }
  
  return true;
}

bool do_base_color_set(const char *set_str) {
  int blockid;
  color c;
  
  if (!parse_set(set_str, blockid, c)) {
    return false;
  }

  mc::MaterialColor[blockid] = c;
  mc::MaterialSideColor[blockid] = mc::MaterialColor[blockid];
  mc::MaterialSideColor[blockid].darken(0x20);
  return true;
}

bool do_side_color_set(const char *set_str) {
  int blockid;
  color c;
  
  if (!parse_set(set_str, blockid, c)) {
    return false;
  }

  mc::MaterialSideColor[blockid] = color(c);
  return true;
}

// Convert a string such as "-30,40,50,30" to the corresponding N,S,E,W integers,
// and fill in the min/max settings.
bool parse_limits(const string& limits_str, settings_t& s) {
  std::vector<std::string> limits;

  boost_split(limits, limits_str);

  if (limits.size() != 4) {
    error << "Limit argument must of format: <N>,<S>,<E>,<W>";
    return false;
  }

  try {
    s.min_x = lexical_cast<int>(limits[0]);
    s.max_x = lexical_cast<int>(limits[1]);
    s.min_z = lexical_cast<int>(limits[2]);
    s.max_z = lexical_cast<int>(limits[3]);
  } catch(const bad_lexical_cast& e) {
    error << "Cannot be converted to set of numbers: " << limits_str;
    return false;
  }

  return true;
}

bool read_set(std::set<string>& set, const string s) {
  boost_split(set, s);

  if (set.size() == 0) {
    error << "List must specify items separated by comma `,'";
    return false;
  }
  
  return true;
}

bool do_write_palette(settings_t& s, string& path) {
  std::ofstream pal(path.c_str());

  pal << "#" << left << setw(20) << "<block-id>" << setw(16) << "<base R,G,B,A>" << " " << setw(16) << "<side R,G,B,A>" << '\n';
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    color mc = mc::MaterialColor[i];
    color msc = mc::MaterialSideColor[i];
    pal << left << setw(20) << mc::MaterialName[i] << " " << setw(16) << mc << " " << setw(16) << msc << '\n';
  }

  if (pal.fail()) {
    error << "Failed to write palette to " << path;
    return false;
  }
  
  return true;
}

bool do_read_palette(settings_t& s, string& path) {
  typedef tokenizer<boost::char_separator<char> > tokenizer;

  std::ifstream pal(path.c_str());
  char_separator<char> sep(" \t\n\r");
  
  while (!pal.eof()) {
    string line;
    getline(pal, line, '\n');
    
    tokenizer tokens(line, sep);
    
    int blockid = 0, i = 0;
    color c;
    
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter, ++i) {
      string token = *tok_iter;
      
      if (token.at(0) == '#') {
        // rest is comment
        break;
      }
      
      switch(i) {
        case 0:
          if (!get_blockid(token, blockid)) {
            return false;
          }
          break;
        case 1:
          if (!parse_color(token, c)) {
            return false;
          }
          
          mc::MaterialColor[blockid] = c;
          c.darken(0x20);
          mc::MaterialSideColor[blockid] = c;
          break;
        case 2:
          if (!parse_color(token, c)) {
            return false;
          }
          
          mc::MaterialSideColor[blockid] = c;
          break;
        default:
          break;
      }
    }
  }
  
  return true;
}

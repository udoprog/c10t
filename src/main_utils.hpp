#ifndef __MAIN_UTILS_HPP__
#define __MAIN_UTILS_HPP__

#include <string>
#include <set>
#include <sstream>
#include <vector>
#include <string>

#include "image/color.hpp"

#include "global.hpp"

using namespace std;

class application_error : std::exception
{
  private:
    const char* message;
  public:
    application_error(const char* message) : message(message) {}

    const char* what() const throw() {
      return message;
    }
};

bool get_blockid(const std::string blockid_string, int& blockid);
bool parse_color(const std::string value, color& c);
bool parse_set(const char* set_str, int& blockid, color& c);
bool do_base_color_set(const char *set_str);
bool do_side_color_set(const char *set_str);

// Convert a string such as "-30,40,50,30" to the corresponding N,S,E,W integers,
// and fill in the min/max settings.
bool parse_limits(const std::string& limits_str, settings_t& s);
bool read_set(std::set<std::string>& set, const std::string s);
bool parse_polyline(const string& limits_str, settings_t& s);

 
bool do_write_palette(settings_t& s, const fs::path& path);
bool do_read_palette(settings_t& s, const fs::path& path);

bool read_opts(settings_t& s, int argc, char* argv[]);

extern std::vector<std::string> hints;
extern std::vector<std::string> warnings;
extern std::stringstream error;

#endif /* __MAIN_UTILS_HPP__ */

#ifndef __MCUTILS_HPP__
#define __MCUTILS_HPP__

#include <exception>

#include <boost/filesystem.hpp>

#include "common.hpp"

namespace fs = boost::filesystem;

class invalid_argument : public std::exception {
  private:
    const char* message;
  public:
    invalid_argument(const char* message) : message(message) {}

    const char* what() const throw() {
      return message;
    }
};

namespace mcutils {
  fs::path level_dir(const fs::path base, int x, int z);
  fs::path level_path(const fs::path base, int x, int z, const std::string prefix, const std::string suffix);
  
  /*
   * Used to indicate a level coordinate.
   */
  struct level_coord {
    int x;
    int z;

    level_coord() : x(0), z(0) {}
  };
  
  /* 
   * Take a path to a level chunk file, and return the coordinates.
   * Indicate that no coordinates are viable by throwing invalid_argument
   **/
  void path_to_level_coord(const fs::path path, level_coord& coord);
};

#endif /* __MCUTILS_HPP__ */

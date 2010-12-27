#ifndef __MCUTILS_HPP__
#define __MCUTILS_HPP__

#include <exception>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace mc {
  namespace utils {
    /**
     * The fastest portable split I could find, only limitation is that it splits on only one character, but that's O.K.
     **/
    void split(std::vector<std::string>& v, const std::string& str, char delim);
    
    class invalid_argument : public std::exception {
      private:
        const char* message;
      public:
        invalid_argument(const char* message) : message(message) {}

        const char* what() const throw() {
          return message;
        }
    };
    
    class bad_cast : public std::exception {};
    
    std::string b36encode(int number);
    int b36decode(const std::string number);
    
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
};

#endif /* __MCUTILS_HPP__ */

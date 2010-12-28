// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_UTILS_HPP__
#define __MC_UTILS_HPP__

#include <exception>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace mc {
  /*
   * designates a text related to a position
   * Possible usages:
   *   Signs
   *   Player Positions
   *   Custom Markers
   **/
  struct marker {
  public:
    std::string text;
    int x, y, z;
    
    marker(std::string text, int x, int y, int z) :
        text(text), x(x), y(y), z(z)
    {
    }
  };
  
  namespace utils {
    /**
     * The fastest portable split I could find, only
     * limitation is that it splits on only one character, but that's O.K.
     * And yes - this actually has an effective impact on performance since
     * it is heavily used during broad phase scanning.
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
      private:
        const int x;
        const int z;
      public:
        level_coord(int x, int z) : x(x), z(z) {}
        
        int get_x() const { return x; };
        int get_z() const { return z; };
        
        level_coord rotate(int rotation) const {
          int x = this->x;
          int z = this->z;
          int t = x;
          
          switch (rotation % 360) {
            case 270:
              x = z;
              z = -t;
              break;
            case 180:
              x = -x;
              z = -z;
              break;
            case 90:
              x = -z;
              z = t;
              break;
          }
          
          return level_coord(x, z);
        }
        
        bool operator<(const level_coord& other) const {
          if (z < other.z) {
            return true;
          }
          
          if (z > other.z) {
            return false;
          }
          
          return x < other.x;
        }
    };
    
    /* 
     * Take a path to a level chunk file, and return the coordinates.
     * Indicate that no coordinates are viable by throwing invalid_argument
     **/
    level_coord path_to_level_coord(const fs::path path);
  }
}

#endif /* __MC_UTILS_HPP__ */

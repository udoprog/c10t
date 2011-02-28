// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_UTILS_HPP__
#define __MC_UTILS_HPP__

#include <exception>

#include <boost/filesystem.hpp>
#include <cstring>

namespace fs = boost::filesystem;

namespace mc {
  /*
   * A dynamic buffer can be used when you need a fixed sized buffer that can
   * be expanded.
   *
   * This is useful if you have a shared buffer of optimal size which might
   * expand, but won't in the normal case.
   */
  class dynamic_buffer {
  private:
    enum {
      DEFAULT_FACTOR_MAX = 16
    };
    int factor;
    int factor_max;
    size_t size;
    size_t buffer_size;
    char* buffer;
  public:
    dynamic_buffer(size_t size)
      : factor(1), factor_max(DEFAULT_FACTOR_MAX),
        size(size), buffer_size(size), buffer(new char[size])
    {
    }

    dynamic_buffer(size_t size, int factor_max)
      : factor(1), factor_max(factor_max),
        size(size), buffer_size(size), buffer(new char[size])
    {
    }

    size_t get_size() {
      return buffer_size;
    }

    char* get() {
      return buffer;
    }

    /**
     * Expand the buffer and return the amount it has been expanded with.
     */
    size_t expand() {
      if (factor >= factor_max) {
        return 0;
      }

      factor += 1;

      size_t new_size = factor * size;
      char* new_buffer = new char[new_size];

      memcpy(new_buffer, buffer, buffer_size);
      delete [] buffer;

      size_t expanded_size = new_size - buffer_size;
      buffer_size = new_size;
      return expanded_size;
    }

    ~dynamic_buffer() {
      delete [] buffer;
    }
  };

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
        int x;
        int z;
      public:
        level_coord() : x(0), z(0) {}
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
    level_coord path_to_region_coord(const fs::path path);
  }
}

#endif /* __MC_UTILS_HPP__ */

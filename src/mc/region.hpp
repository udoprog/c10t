// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _MC_REGION_HPP_
#define _MC_REGION_HPP_

#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>

#include "mc/utils.hpp"

#include <fstream>
#include <cstring>
#include <exception>

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
      delete[] buffer;

      size_t expanded_size = new_size - buffer_size;

      buffer = new_buffer;
      buffer_size = new_size;
      return expanded_size;
    }

    ~dynamic_buffer() {
      delete [] buffer;
    }
  };

  class bad_region : public std::exception {
    private:
      const fs::path path;
      const char* message;
    public:
      bad_region(const fs::path path, const char* message) 
        : path(path), message(message)
      {
      }

      ~bad_region() throw() {  }

      const char* what() const throw() {
        return message;
      }
      
      const fs::path where() {
        return path;
      }
  };

  typedef struct chunk_offset {
    uint32_t sector_number;
    uint8_t sector_count;
  } chunk_offset;

  class region;

  typedef boost::shared_ptr<region> region_ptr;

  class region {
  public:
    enum {
      HEADER_SIZE = 8192,
      REGION_SIZE = 32,
      HEADER_RECORD_SIZE = 4,
      RECORD_SIZE = 4096,
      CHUNK_MAX = 1024 * 128
    } constants;
  private:
    fs::path path;
    boost::shared_array<char> header;
    dynamic_buffer in_buffer;
  public:
    region(fs::path path);

    void read_header();
    inline unsigned int get_offset(unsigned int x, unsigned int z) const;

    chunk_offset read_chunk_offset(unsigned int x, unsigned int z) const;

    template<typename T>
    void read_coords(T& coll) const
    {
      using mc::utils::level_coord;

      for (int z = 0; z < REGION_SIZE; z++) {
        for (int x = 0; x < REGION_SIZE; x++) {
          chunk_offset co = read_chunk_offset(x, z);

          if (co.sector_number != 0) {
            level_coord c(x, z);
            coll.push_back(c);
          }
        }
      }
    }

    uint32_t read_data(int x, int z, dynamic_buffer& buffer);
    fs::path get_path();
  };
}

#endif /*_MC_REGION_HPP_*/

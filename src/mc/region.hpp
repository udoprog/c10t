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

  class region {
  public:
    enum {
      HEADER_SIZE = 8192,
      RECORD_MULTI = 256,
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

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _MC_REGION_HPP_
#define _MC_REGION_HPP_

#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>

#include "mc/utils.hpp"
#include <sstream>

#include <fstream>
#include <cstring>

#include <zlib.h>

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
      REGION_SIZE = 32,
      HEADER_RECORD_SIZE = 4,
      CHUNK_MAX = 1024 * 128
    } constants;
  private:
    fs::path path;
    boost::shared_array<char> header;
  public:
    region(fs::path path) : path(path), header(new char[HEADER_SIZE]) {
      std::ifstream fp(path.string().c_str());
      fp.read(header.get(), HEADER_SIZE);
    }

    inline unsigned int get_offset(unsigned int x, unsigned int z) const
    {
      return 4 * ((x&31) + (z&31) * REGION_SIZE);
    }

    chunk_offset read_chunk_offset(unsigned int x, unsigned int z) const
    {
      int o = get_offset(x, z);

      uint8_t buf[HEADER_RECORD_SIZE];

      ::memcpy(reinterpret_cast<char*>(buf), &header[o], HEADER_RECORD_SIZE);

      chunk_offset co;

      co.sector_number = ((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8)) >> 8;
      co.sector_count = uint8_t(buf[3]);

      return co;
    }

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

    std::string read_data(int x, int z) const
    {
      chunk_offset co = read_chunk_offset(x, z);
      
      uint8_t buf[5];

      std::ifstream fp(path.string().c_str());
      fp.seekg(4096 * co.sector_number);
      fp.read(reinterpret_cast<char*>(buf), 5);

      if (fp.fail()) {
        throw bad_region(path, "could not read chunk header");
      }

      uint32_t len = buf[0]<<24|buf[1]<<16|buf[2]<<8|buf[3];
      uint8_t version = uint8_t(buf[4]);

      if (version != 2) {
        throw bad_region(path, "bad chunk version");
      }

      if (len > co.sector_count * 4096 || len > CHUNK_MAX) {
        throw bad_region(path, "invalid chunk length");
      }

      char in[CHUNK_MAX], data[CHUNK_MAX];

      fp.read(in, len);

      if (fp.fail()) {
        throw bad_region(path, "could not read chunk");
      }

      z_stream strm;

      strm.zalloc = (alloc_func)NULL;
      strm.zfree = (free_func)NULL;
      strm.opaque = NULL;

      strm.next_in = reinterpret_cast<Bytef*>(in);
      strm.avail_in = len - 1;

      inflateInit(&strm);

      std::stringstream oss;

      strm.next_out = reinterpret_cast<Bytef*>(data);
      strm.avail_out = sizeof(data);

      int status = inflate(&strm, Z_FINISH);

      if (status != Z_STREAM_END) {
        inflateEnd(&strm);
        throw bad_region(path, "failed to inflate data (Z_STREAM_END)");
      }
        
      oss.write(data, CHUNK_MAX - strm.avail_out);

      inflateEnd(&strm);
      return oss.str();
    }

    fs::path get_path() {
      return path;
    }
  };
}

#endif /*_MC_REGION_HPP_*/

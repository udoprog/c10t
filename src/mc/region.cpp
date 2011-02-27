#include "mc/region.hpp"

#include <zlib.h>

#include <fstream>

namespace mc {
  region::region(fs::path path)
      : path(path)
  {
    char* header_c = new char[HEADER_SIZE];
    header.reset(header_c);

    std::fstream fp(path.string().c_str(), std::ios::in | std::ios::binary);
    fp.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

    if (fp.fail()) {
      throw bad_region(path, "failed to open region");
    }

    fp.read(header_c, HEADER_SIZE);

    if (fp.fail()) {
      throw bad_region(path, "failed to read header area");
    }
  }

  inline unsigned int region::get_offset(unsigned int x, unsigned int z) const
  {
    return 4 * ((x&31) + (z&31) * REGION_SIZE);
  }

  chunk_offset region::read_chunk_offset(unsigned int x, unsigned int z) const
  {
    int o = get_offset(x, z);

    uint8_t buf[HEADER_RECORD_SIZE];

    ::memcpy(reinterpret_cast<char*>(buf), &header[o], HEADER_RECORD_SIZE);

    chunk_offset co;

    co.sector_number = ((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8)) >> 8;
    co.sector_count = uint8_t(buf[3]);

    return co;
  }

  uint32_t region::read_data(int x, int z, char* buffer, uint32_t buffer_size) const
  {
    chunk_offset co = read_chunk_offset(x, z);
    
    uint8_t buf[5];

    std::fstream fp(path.string().c_str(), std::ios::in | std::ios::binary);

    if (fp.fail()) {
      throw bad_region(path, "failed to open file");
    }

    fp.seekg(RECORD_SIZE * co.sector_number);

    if (fp.fail()) {
      throw bad_region(path, "could not seek");
    }

    fp.read(reinterpret_cast<char*>(buf), 5);

    if (fp.fail()) {
      throw bad_region(path, "could not read chunk header");
    }

    uint32_t len = (buf[0]<<24) + (buf[1]<<16) + (buf[2]<<8) + (buf[3]);
    uint8_t version = buf[4];

    if (len > buffer_size) {
      throw bad_region(path, "chunk too large to fit in data pointer, use CHUNK_MAX instead");
    }

    if (version != 2) {
      throw bad_region(path, "bad chunk version");
    }

    char read_buffer[CHUNK_MAX];

    fp.read(read_buffer, len);

    if (fp.fail()) {
      throw bad_region(path, "could not read chunk");
    }

    fp.close();

    z_stream strm;

    strm.zalloc = (alloc_func)NULL;
    strm.zfree = (free_func)NULL;
    strm.opaque = NULL;

    strm.next_in = reinterpret_cast<Bytef*>(read_buffer);
    strm.avail_in = len - 1;

    inflateInit(&strm);

    strm.next_out = reinterpret_cast<Bytef*>(buffer);
    strm.avail_out = buffer_size;

    int status = inflate(&strm, Z_FINISH);

    inflateEnd(&strm);

    switch (status) {
      case Z_ERRNO:
        throw bad_region(path, "failed to inflate data (Z_ERRNO)");
      case Z_STREAM_ERROR:
        throw bad_region(path, "failed to inflate data (Z_STREAM_ERROR)");
      case Z_DATA_ERROR:
        throw bad_region(path, "failed to inflate data (Z_DATA_ERROR)");
      case Z_MEM_ERROR:
        throw bad_region(path, "failed to inflate data (Z_MEM_ERROR)");
      case Z_BUF_ERROR:
        throw bad_region(path, "failed to inflate data (Z_BUF_ERROR)");
      case Z_VERSION_ERROR:
        throw bad_region(path, "failed to inflate data (Z_VERSION_ERROR)");
      default:
        break;
    }

    if (strm.avail_in > 0) {
      throw bad_region(path, "all data was not inflated");
    }

    return buffer_size - strm.avail_out;
  }

  fs::path region::get_path() {
    return path;
  }
}

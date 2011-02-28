#include "mc/region.hpp"

#include <zlib.h>

#include <fstream>

namespace mc {
  region::region(fs::path path)
      : path(path), in_buffer(CHUNK_MAX)
  {
  }

  void region::read_header()
  {
    header.reset(new char[HEADER_SIZE]);

    std::fstream fp(path.string().c_str(), std::ios::in | std::ios::binary);

    if (fp.fail()) {
      throw bad_region(path, "failed to open region");
    }

    fp.read(header.get(), HEADER_SIZE);

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
    if (!header) {
      throw bad_region(path, "header has not been loaded");
    }

    int o = get_offset(x, z);

    uint8_t buf[HEADER_RECORD_SIZE];

    ::memcpy(reinterpret_cast<char*>(buf), &header[o], HEADER_RECORD_SIZE);

    chunk_offset co;

    co.sector_number = ((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8)) >> 8;
    co.sector_count = uint8_t(buf[3]);

    return co;
  }

  uint32_t region::read_data(int x, int z, dynamic_buffer& buffer)
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

    if (version != 2) {
      throw bad_region(path, "bad chunk version");
    }

    while (len > in_buffer.get_size()) {
      if (in_buffer.expand() == 0) {
        throw bad_region(path, "region input buffer requires too much memory");
      }
    }

    fp.read(in_buffer.get(), len);

    if (fp.fail()) {
      throw bad_region(path, "could not read chunk");
    }

    fp.close();

    z_stream strm;

    strm.zalloc = (alloc_func)NULL;
    strm.zfree = (free_func)NULL;
    strm.opaque = NULL;

    strm.next_in = reinterpret_cast<Bytef*>(in_buffer.get());
    strm.avail_in = len - 1;

    inflateInit(&strm);

    strm.avail_out = buffer.get_size();

    size_t pos = 0;

    do {
      strm.next_out = reinterpret_cast<Bytef*>(buffer.get() + pos);

      int status = inflate(&strm, Z_NO_FLUSH);

      switch (status) {
        case Z_ERRNO:
          inflateEnd(&strm);
          throw bad_region(path, "failed to inflate data (Z_ERRNO)");
        case Z_STREAM_ERROR:
          inflateEnd(&strm);
          throw bad_region(path, "failed to inflate data (Z_STREAM_ERROR)");
        case Z_DATA_ERROR:
          inflateEnd(&strm);
          throw bad_region(path, "failed to inflate data (Z_DATA_ERROR)");
        case Z_MEM_ERROR:
          inflateEnd(&strm);
          throw bad_region(path, "failed to inflate data (Z_MEM_ERROR)");
        case Z_BUF_ERROR:
          inflateEnd(&strm);
          throw bad_region(path, "failed to inflate data (Z_BUF_ERROR)");
        case Z_VERSION_ERROR:
          inflateEnd(&strm);
          throw bad_region(path, "failed to inflate data (Z_VERSION_ERROR)");
        default:
          break;
      }

      if (strm.avail_in > 0) {
        strm.avail_out = buffer.expand();

        if (strm.avail_out == 0) {
          throw bad_region(path, "region output buffer requires too much memory");
        }

        pos += strm.avail_out;
      }
    } while(strm.avail_in > 0);

    inflateEnd(&strm);

    return buffer.get_size() - strm.avail_out;
  }

  fs::path region::get_path() {
    return path;
  }
}

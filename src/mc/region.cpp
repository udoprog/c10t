#include "mc/region.hpp"

#include <zlib.h>

namespace mc {
  region::region(fs::path path)
      : path(path)
  {
    char* header_c = new char[HEADER_SIZE];
    header.reset(header_c);

    std::ifstream fp(path.string().c_str());
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

  void region::read_data(int x, int z, std::stringstream& oss) const
  {
    chunk_offset co = read_chunk_offset(x, z);
    
    uint8_t buf[5];

    std::ifstream fp(path.string().c_str());

    if (fp.fail()) {
      throw bad_region(path, "failed to open file");
    }

    fp.seekg(4096 * co.sector_number);

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

    boost::shared_array<char> in(new char[len]);

    fp.read(in.get(), len);

    if (fp.fail()) {
      throw bad_region(path, "could not read chunk");
    }

    fp.close();

    z_stream strm;

    strm.zalloc = (alloc_func)NULL;
    strm.zfree = (free_func)NULL;
    strm.opaque = NULL;

    strm.next_in = reinterpret_cast<Bytef*>(in.get());
    strm.avail_in = len - 1;

    inflateInit(&strm);

    int status;

    char data[CHUNK_MAX];

    do {
      strm.next_out = reinterpret_cast<Bytef*>(data);
      strm.avail_out = CHUNK_MAX;

      status = inflate(&strm, Z_NO_FLUSH);

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
      
      oss.write(data, CHUNK_MAX - strm.avail_out);

      if (status == Z_STREAM_END) {
        break;
      }

      if (status == Z_NEED_DICT) {
        inflateEnd(&strm);
        throw bad_region(path, "unhandled inflate state (Z_NEED_DICT)");
      }
    } while (strm.avail_in > 0);

    inflateEnd(&strm);
  }

  fs::path region::get_path() {
    return path;
  }
}

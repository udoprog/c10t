#include "mc/region.hpp"

#include <zlib.h>

#include <fstream>
#include <boost/scoped_ptr.hpp>

namespace mc {
  class zerror : public std::exception {
    private:
      const char* message;
    public:
      zerror(const char* message) 
        : message(message)
      {
      }

      ~zerror() throw() {  }

      const char* what() const throw() {
        return message;
      }
  };

  class zstream 
  {
  public:
    zstream() : strm(new z_stream)
    {
      strm->zalloc = (alloc_func)NULL;
      strm->zfree = (free_func)NULL;
      strm->opaque = NULL;
      inflateInit(strm.get());
    }

    ~zstream()
    {
      inflateEnd(strm.get());
    }

    uint32_t get_avail_out()
    {
      return strm->avail_out;
    }

    uint32_t get_avail_in()
    {
      return strm->avail_in;
    }

    void set_in(Bytef* b, uint32_t len)
    {
      strm->next_in = b;
      strm->avail_in = len;
    }

    void set_out(Bytef* b, uint32_t len)
    {
      strm->next_out = b;
      strm->avail_out = len;
    }

    bool in_empty()
    {
      return strm->avail_in <= 0;
    }

    bool out_empty()
    {
      return strm->avail_out <= 0;
    }

    void inflate()
    {
      int status = ::inflate(strm.get(), Z_NO_FLUSH);

      switch (status) {
        case Z_ERRNO:
          throw zerror("failed to inflate data (Z_ERRNO)");
        case Z_STREAM_ERROR:
          throw zerror("failed to inflate data (Z_STREAM_ERROR)");
        case Z_DATA_ERROR:
          throw zerror("failed to inflate data (Z_DATA_ERROR)");
        case Z_MEM_ERROR:
          throw zerror("failed to inflate data (Z_MEM_ERROR)");
        case Z_BUF_ERROR:
          throw zerror("failed to inflate data (Z_BUF_ERROR)");
        case Z_VERSION_ERROR:
          throw zerror("failed to inflate data (Z_VERSION_ERROR)");
        default:
          break;
      }
    }
  private:
    boost::scoped_ptr<z_stream> strm;
  };

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

    zstream strm;

    size_t pos = 0;

    strm.set_in(reinterpret_cast<Bytef*>(in_buffer.get()), len - 1);
    strm.set_out(reinterpret_cast<Bytef*>(buffer.get() + pos), buffer.get_size());

    do {
      try {
        strm.inflate();
      } catch(const zerror& e) {
        throw bad_region(path, e.what());
      }

      if (!strm.in_empty()) {
        uint32_t expanded = buffer.expand();

        if (expanded == 0) {
          throw bad_region(path, "region output buffer requires too much memory");
        }

        pos += expanded;
        strm.set_out(reinterpret_cast<Bytef*>(buffer.get() + pos), expanded);
      }
    } while(!strm.in_empty());

    return buffer.get_size() - strm.get_avail_out();
  }

  fs::path region::get_path() {
    return path;
  }
}

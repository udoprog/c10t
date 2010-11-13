#ifndef _CACHE_H_
#define _CACHE_H_

#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>
#include <zlib.h>

#include "image/image_operations.hpp"

namespace fs = boost::filesystem;

#define CACHE_MAGIC "CMap"

struct cache_hdr {
  bool compressed;
  std::time_t mod;
  uintmax_t filesize;
  size_t minx, miny;
  size_t maxx, maxy;
  size_t size;
};

class cache_file {
private:
  const fs::path cache_dir;
  const fs::path source_path;
  const bool cache_compress;
  
  const fs::path cache_path;
  
  typedef std::vector<image_operation>::size_type v_size_type;
  
public:
  cache_file(const fs::path cache_dir, const fs::path source_path, bool cache_compress)
    : cache_dir(cache_dir), source_path(source_path), 
      cache_compress(cache_compress),
      cache_path(cache_dir / (fs::basename(source_path) + ".cmap"))
  {
  }
  
  bool exists() {
    return fs::is_regular(cache_path)
      && fs::last_write_time(cache_path) >= fs::last_write_time(source_path);
  }

  void clear() {
    fs::remove(cache_path);
  }
  
  bool read(boost::shared_ptr<image_operations> oper) {
    std::ifstream fs(cache_path.string().c_str());
    
    cache_hdr hdr;
    
    {
      char m[4];
      fs.read(m, 4);
      if (fs.fail()) return false;

      if (
            m[0] != CACHE_MAGIC[0]
        ||  m[1] != CACHE_MAGIC[1]
        ||  m[2] != CACHE_MAGIC[2]
        ||  m[3] != CACHE_MAGIC[3]
        ) return false;
      
      fs.read(reinterpret_cast<char*>(&hdr), sizeof(cache_hdr));
    }
      
    if (hdr.compressed != cache_compress) return false;
    if (hdr.mod != fs::last_write_time(source_path)) return false;
    if (hdr.filesize != fs::file_size(source_path)) return false;
    
    oper->maxx = hdr.maxx;
    oper->minx = hdr.minx;
    oper->maxy = hdr.maxy;
    oper->miny = hdr.miny;
    oper->operations.resize(hdr.size);
    
    fs.read(reinterpret_cast<char*>(&(oper->operations.front())), sizeof(image_operation) * hdr.size);
    if (fs.fail()) return false;
    
    return true;
  }
  
  bool write(boost::shared_ptr<image_operations> oper) {
    std::ofstream fs(cache_path.string().c_str());
    
    cache_hdr hdr;
    
    {
      fs.write(CACHE_MAGIC, 4);
      if (fs.fail()) return false;
      
      hdr.compressed = cache_compress;
      hdr.maxx = oper->maxx;
      hdr.minx = oper->minx;
      hdr.maxy = oper->maxy;
      hdr.miny = oper->miny;
      hdr.mod = fs::last_write_time(source_path);
      hdr.filesize = fs::file_size(source_path);
      hdr.size = oper->operations.size();
      
      fs.write(reinterpret_cast<char*>(&hdr), sizeof(cache_hdr));
    }
    
    fs.write(reinterpret_cast<char*>(&(oper->operations.front())), sizeof(image_operation) * hdr.size);
    if (fs.fail()) return false;
    
    return true;
  }
};

#endif /* _CACHE_H_ */

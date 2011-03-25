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
  size_t min_x, min_y;
  size_t max_x, max_y;
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
  
  bool create_directories() {
    return fs::create_directories(cache_dir);
  }
  
  bool exists() {
    return fs::is_regular(cache_path)
      && fs::last_write_time(cache_path) >= fs::last_write_time(source_path);
  }

  void clear() {
    fs::remove(cache_path);
  }
  
  bool gzreadall(gzFile gzf, char* buf, unsigned int len) {
    unsigned int read = 0;
    while (read < len) {
      int have = gzread(gzf, buf + read, len - read);

      if (have == 0) {
        int errnum;
        const char* errorstr = gzerror(gzf, &errnum);
        
        if (errnum != 0) {
          std::cerr << errorstr << std::endl;
          gzclose(gzf);
          return false;
        }
        
        if (gzeof(gzf)) {
          gzclose(gzf);
          return false;
        }
      }
      
      if (have < 0) {
        gzclose(gzf);
        return false;
      }
      
      read += have;
    }
    return true;
  }
  
  bool gzwriteall(gzFile gzf, const char* buf, unsigned int len) {
    unsigned int written = 0;
    while (written < len) {
      int have = gzwrite(gzf, buf + written, len - written);
      
      if (have <= 0) {
        int errnum;
        const char* errorstr = gzerror(gzf, &errnum);
        
        if (errnum != 0) {
          std::cerr << errorstr << std::endl;
          gzclose(gzf);
          return false;
        }
        
        gzclose(gzf);
        return false;
      }
      
      written += have;
    }
    return true;
  }
  
  bool read(boost::shared_ptr<image_operations> oper) {
    gzFile gzf = gzopen(cache_path.string().c_str(), "r");

    if (gzf == Z_NULL) {
      return false;
    }

    //std::ifstream fs(cache_path.string().c_str());
    
    cache_hdr hdr;
    
    {
      char m[4];
      //fs.read(m, 4);
      //if (fs.fail()) return false;
      
      if (!gzreadall(gzf, m, 4)) {
        return false;
      }
      
      if (
            m[0] != CACHE_MAGIC[0]
        ||  m[1] != CACHE_MAGIC[1]
        ||  m[2] != CACHE_MAGIC[2]
        ||  m[3] != CACHE_MAGIC[3]
        ) return false;
      
      //fs.read(reinterpret_cast<char*>(&hdr), sizeof(cache_hdr));
      //if (fs.fail()) return false;
      
      if (!gzreadall(gzf, reinterpret_cast<char*>(&hdr), sizeof(cache_hdr))) {
        return false;
      }
    }
      
    if (hdr.compressed != cache_compress) return false;
    if (hdr.mod != fs::last_write_time(source_path)) return false;
    if (hdr.filesize != fs::file_size(source_path)) return false;
    
    oper->max_x = hdr.max_x;
    oper->min_x = hdr.min_x;
    oper->max_y = hdr.max_y;
    oper->min_y = hdr.min_y;
    oper->operations.resize(hdr.size);
    
    //fs.read(reinterpret_cast<char*>(&(oper->operations.front())), sizeof(image_operation) * hdr.size);
    //if (fs.fail()) return false;
    
    if (!gzreadall(gzf, reinterpret_cast<char*>(&(oper->operations.front())), sizeof(image_operation) * hdr.size)) {
      return false;
    }
    
    gzclose(gzf);
    return true;
  }
  
  bool write(boost::shared_ptr<image_operations> oper) {
    //std::ofstream fs(cache_path.string().c_str());
    gzFile gzf = gzopen(cache_path.string().c_str(), "w");
    
    if (gzf == Z_NULL) {
      return false;
    }
    
    cache_hdr hdr;
    
    {
      //fs.write(CACHE_MAGIC, 4);
      //if (fs.fail()) return false;
      if (!gzwriteall(gzf, CACHE_MAGIC, 4)) {
        return false;
      }
      
      hdr.compressed = cache_compress;
      hdr.max_x = oper->max_x;
      hdr.min_x = oper->min_x;
      hdr.max_y = oper->max_y;
      hdr.min_y = oper->min_y;
      hdr.mod = fs::last_write_time(source_path);
      hdr.filesize = fs::file_size(source_path);
      hdr.size = oper->operations.size();
      
      //fs.write(reinterpret_cast<char*>(&hdr), sizeof(cache_hdr));
      //if (fs.fail()) return false;
      if (!gzwriteall(gzf, reinterpret_cast<char*>(&hdr), sizeof(cache_hdr))) {
        return false;
      }
    }
    
    //fs.write(reinterpret_cast<char*>(&(oper->operations.front())), sizeof(image_operation) * hdr.size);
    //if (fs.fail()) return false;
    
    if (!gzwriteall(gzf, reinterpret_cast<char*>(&(oper->operations.front())), sizeof(image_operation) * hdr.size)) {
      return false;
    }
    
    gzclose(gzf);
    return true;
  }
};

#endif /* _CACHE_H_ */

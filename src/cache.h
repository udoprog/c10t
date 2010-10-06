#ifndef _CACHE_H_
#define _CACHE_H_

#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>
#include <zlib.h>

#include "image.h"

namespace fs = boost::filesystem;

struct imop_file {
  uint16_t x, y;
  color c;
};

class cache_file {
private:
  bool cache_compress;
  fs::path path;
  std::time_t modification_time;
  typedef std::vector<image_operation>::size_type v_size_type;

  inline bool gzwriteall(gzFile fp, char* bytes, size_t size) {
    size_t size_o = 0;
    
    while (size_o < size) {
      size_t size_w = gzwrite(fp, bytes + size_o, size - size_o);
      if (size_w <= 0) return false;
      size_o += size_w;
    }
    
    return true;
  }

  inline bool gzreadall(gzFile fp, char* bytes, size_t size) {
    size_t size_o = 0;
    
    while (size_o < size) {
      size_t size_w = gzread(fp, bytes + size_o, size - size_o);
      if (size_w <= 0) return false;
      size_o += size_w;
    }
    
    return true;
  }
  
  bool write_z(image_operations* operations) {
    gzFile fp = gzopen(path.string().c_str(), "wb");
    
    if (fp == Z_NULL) {
      return false;
    }
    
    if (!gzwriteall(fp, reinterpret_cast<char *>(&modification_time), sizeof(std::time_t))) {
      gzclose(fp);
      return false;
    }
    
    operations->optimize();
    
    v_size_type size = operations->operations.size();
    if (!gzwriteall(fp, reinterpret_cast<char *>(&size), sizeof(v_size_type))) {
      gzclose(fp);
      return false;
    }
    
    for (
      std::vector<image_operation>::iterator it = operations->operations.begin();
      it != operations->operations.end(); it++) {
      image_operation oper = *it;
      imop_file iof;
      iof.x = oper.x;
      iof.y = oper.y;
      iof.c = oper.c;
      if (!gzwriteall(fp, reinterpret_cast<char*>(&iof), sizeof(imop_file))) {
        gzclose(fp);
        return false;
      }
    }
    
    gzclose(fp);
    return true;
  }
  
  bool read_z(image_operations* operations, std::time_t mod) {
    gzFile fp = gzopen(path.string().c_str(), "rb");
    
    if (!gzreadall(fp, reinterpret_cast<char*>(&modification_time), sizeof(std::time_t))) {
      gzclose(fp);
      return false;
    }
    
    if (modification_time != mod) {
      gzclose(fp);
      return false;
    }
    
    v_size_type size;
    if (!gzreadall(fp, reinterpret_cast<char*>(&size), sizeof(v_size_type)))
    {
      gzclose(fp);
      return false;
    }
    
    imop_file* oper = new imop_file[size];
    
    if (!gzreadall(fp, reinterpret_cast<char*>(oper), sizeof(imop_file) * size)) {
      gzclose(fp);
      delete [] oper;
      return false;
    }

    for (v_size_type i = 0; i < size; i++) {
      imop_file iof = oper[i];
      image_operation operation;
      operation.x = iof.x;
      operation.y = iof.y;
      operation.c = iof.c;
      operation.order = i;
      operations->operations.push_back(operation);
    }
    
    delete [] oper;
    gzclose(fp);
    return true;
  }
public:
  cache_file(settings_t& s) : cache_compress(s.cache_compress) {
  }
  
  fs::path get_path() {
    return path;
  }

  void set_path(fs::path path) {
    this->path = path;
  }

  std::time_t get_modification_time() {
    return modification_time;
  }

  void set_modification_time(std::time_t modification_time) {
    this->modification_time = modification_time;
  }
  
  bool read(image_operations* operations, std::time_t mod) {
    if (cache_compress) return read_z(operations, mod);
    
    std::ifstream fs(path.string().c_str());
    fs.read(reinterpret_cast<char*>(&modification_time), sizeof(std::time_t));
    if (fs.fail()) return false;
    if (modification_time != mod) return false;
    
    v_size_type size;
    fs.read(reinterpret_cast<char*>(&size), sizeof(v_size_type));
    if (fs.fail()) return false;
    
    imop_file* oper = new imop_file[size];
    fs.read(reinterpret_cast<char*>(oper), sizeof(imop_file) * size);
    if (fs.fail()) goto exit_error;
    
    for (v_size_type i = 0; i < size; i++) {
      imop_file iof = oper[i];
      image_operation operation;
      operation.x = iof.x;
      operation.y = iof.y;
      operation.c = iof.c;
      operation.order = i;
      operations->operations.push_back(operation);
    }
    
    delete [] oper;
    return true;
exit_error:
    delete [] oper;
    return false;
  }

  bool write(image_operations* operations) {
    if (cache_compress) return write_z(operations);
    
    std::ofstream fs(path.string().c_str());
    fs.write(reinterpret_cast<char *>(&modification_time), sizeof(std::time_t));
    if (fs.fail()) return false;

    operations->optimize();
    
    v_size_type size = operations->operations.size();
    fs.write(reinterpret_cast<char *>(&size), sizeof(v_size_type));
    if (fs.fail()) return false;
    
    for (
      std::vector<image_operation>::iterator it = operations->operations.begin();
      it != operations->operations.end(); it++) {
      image_operation oper = *it;
      imop_file iof;
      iof.x = oper.x;
      iof.y = oper.y;
      iof.c = oper.c;
      fs.write(reinterpret_cast<char*>(&iof), sizeof(imop_file));
      if (fs.fail()) return false;
    }
    
    return true;
  }
};

#endif /* _CACHE_H_ */

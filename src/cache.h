#ifndef _CACHE_H_
#define _CACHE_H_

#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>

#include "image.h"

namespace fs = boost::filesystem;

struct imop_file {
  uint16_t x, y;
  color c;
};

class cache_file {
private:
    fs::path path;
    std::time_t modification_time;
    typedef std::vector<image_operation>::size_type v_size_type;
public:
    cache_file() {
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
      std::ofstream fs(path.string().c_str());
      fs.write(reinterpret_cast<char *>(&modification_time), sizeof(std::time_t));
      if (fs.fail()) return false;
      
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

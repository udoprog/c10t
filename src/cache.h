#ifndef _CACHE_H_
#define _CACHE_H_

#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>

#include "image.h"

namespace fs = boost::filesystem;

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
      
      image_operation* oper = new image_operation[size];
      fs.read(reinterpret_cast<char*>(oper), sizeof(image_operation) * size);
      if (fs.fail()) goto exit_error;
      
      for (v_size_type i = 0; i < size; i++) {
        operations->operations.push_back(oper[i]);
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
        
        fs.write(reinterpret_cast<char*>(&oper), sizeof(image_operation));
        if (fs.fail()) return false;
      }
      
      return true;
    }
};

#endif /* _CACHE_H_ */

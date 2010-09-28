#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <string>
#include <queue>

#include <dirent.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class dirlist {
private:
  std::queue<fs::path> directories;
  std::queue<fs::path> files;

public:
  dirlist(const fs::path path) {
    if (fs::is_directory(path))
      directories.push(path);
  }
  
  bool has_next() {
    if (!files.empty()) {
      return true;
    }

    if (directories.empty()) {
      return false;
    }
    
    // work until you find any files
    while (!directories.empty()) {
      fs::path dir_path = directories.front();
      directories.pop();
      
      if (!fs::is_directory(dir_path)) {
        return false;
      }
      
      fs::directory_iterator end_itr;
      for ( fs::directory_iterator itr( dir_path );
            itr != end_itr;
            ++itr )
      {
        if (fs::is_directory(itr->status())) {
          directories.push(itr->path());
        }
        else if (fs::is_regular_file(itr->status())) {
          files.push(itr->path());
        }
      }
    }
    
    return !files.empty();
  }

  fs::path next() {
    fs::path next = files.front();
    files.pop();
    return next;
  }
};

#endif /* _FILEUTILS_H_ */

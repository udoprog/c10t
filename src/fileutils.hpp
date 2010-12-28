// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
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
  
  bool has_next(bool (filter)(const std::string&), bool (file_filter)(const std::string&)) {
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
        continue;
      }
      
      fs::directory_iterator end_itr;
      
      for ( fs::directory_iterator itr( dir_path );
            itr != end_itr;
            ++itr )
      {
        if (fs::is_directory(itr->status())) {
          if (!filter(fs::basename(itr->path()))) {
            continue;
          }
          
          directories.push(itr->path());
        }
        else if (fs::is_regular_file(itr->status())) {
          if (!file_filter(itr->path().filename())) {
            continue;
          }
          
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

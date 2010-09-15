#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <string>
#include <queue>

#include <dirent.h>

bool is_dir(std::string &path);

bool is_file(std::string &path);

std::string path_join(std::string a, std::string b);

class dirlist {
private:
  std::queue<std::string> directories;
  std::queue<std::string> files;

public:
  dirlist(std::string path) {
    directories.push(path);
  }
  
  bool hasnext() {
    if (!files.empty()) {
      return true;
    }

    if (directories.empty()) {
      return false;
    }
    
    // work until you find any files
    while (!directories.empty()) {
      std::string path = directories.front();
      directories.pop();
      
      DIR *dir = opendir(path.c_str()); 
      
      if (!dir) {
        return false;
      }
      
      dirent *ent; 
      
      while((ent = readdir(dir)) != NULL)
      {
        std::string temp_str = ent->d_name;

        if (temp_str.compare(".") == 0) {
          continue;
        }
        
        if (temp_str.compare("..") == 0) {
          continue;
        }
        
        std::string fullpath = path_join(path, temp_str);
        
        if (is_dir(fullpath)) {
          directories.push(fullpath);
        }
        else if (is_file(fullpath)) {
          files.push(fullpath);
        }
      }
      
      closedir(dir);
    }
    
    return !files.empty();
  }

  std::string next() {
    std::string next = files.front();
    files.pop();
    return next;
  }
};

#endif /* _FILEUTILS_H_ */

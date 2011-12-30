#ifndef _DIRLIST_HPP
#define _DIRLIST_HPP

#include <queue>
#include <string>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class dirlist {
public:
  typedef bool (dir_filter_func)(const std::string&);
  typedef bool (file_filter_func)(const std::string&);

  dirlist(const fs::path path);
  
  bool has_next(dir_filter_func, file_filter_func);

  fs::path next();
private:
  std::queue<fs::path> directories;
  std::queue<fs::path> files;
};

#endif /* _DIRLIST_HPP */

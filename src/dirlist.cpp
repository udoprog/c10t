#include "dirlist.hpp"
#include "fileutils.hpp"

dirlist::dirlist(const fs::path path) {
  if (fs::is_directory(path))
    directories.push(path);
}

bool dirlist::has_next(dir_filter_func dir_filter, file_filter_func file_filter)
{
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

    for ( fs::directory_iterator itr(dir_path);
          itr != end_itr;
          ++itr )
    {
      if (fs::is_directory(itr->status())) {
        if (!dir_filter(fs::basename(itr->path()))) {
          continue;
        }
        
        directories.push(itr->path());
      }
      else if (fs::is_regular_file(itr->status())) {
        if (!file_filter(path_string(itr->path().filename()))) {
          continue;
        }
        
        files.push(itr->path());
      }
    }
  }
  
  return !files.empty();
}

fs::path dirlist::next()
{
  fs::path next = files.front();
  files.pop();
  return next;
}

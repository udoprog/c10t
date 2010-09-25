#include "fileutils.h"

#include <sys/stat.h>

#include <boost/algorithm/string.hpp>

#ifdef _WIN32
const char *dir_sep = "\\";
const char dir_sep_c = '\\';
#else
const char *dir_sep = "/";
const char dir_sep_c = '/';
#endif

bool is_dir(std::string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   return S_ISDIR(st.st_mode);
}

bool is_file(std::string &path) {
   struct stat st;
   stat(path.c_str(), &st);
   return S_ISREG(st.st_mode);
}

std::string path_join(std::string a, std::string b) {
  return a + dir_sep + b;
}

std::vector<std::string> path_split(std::string path)
{
  std::vector<std::string> result;
  boost::split(result, path, boost::is_any_of(dir_sep));
  return result;
}

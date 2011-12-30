#ifndef _THREADS_RENDERER_SETTINGS_HPP
#define _THREADS_RENDERER_SETTINGS_HPP

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

struct renderer_settings {
  bool cache_use;
  fs::path cache_dir;
  bool cache_compress;
};

#endif /* _THREADS_RENDERER_SETTINGS_HPP */

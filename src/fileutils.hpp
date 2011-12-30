// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _FILEUTILS_HPP
#define _FILEUTILS_HPP

#include <string>

#include <boost/filesystem.hpp>
#include <boost/version.hpp>

#define BOOST_FSv2 ((BOOST_VERSION / 100) <= 1045)

namespace fs = boost::filesystem;

inline std::string path_string(fs::path path) {
#if BOOST_FSv2
  return path;
#else
  return path.string();
#endif
}

#endif /* _FILEUTILS_HPP */

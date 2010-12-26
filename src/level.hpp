// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include "global.hpp"

#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <math.h>
#include <zlib.h>
#include <time.h>
#include <vector>
#include <exception>

#include "nbt/nbt.hpp"

#include "global.hpp"
#include "blocks.hpp"
#include "marker.hpp"

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

namespace fs = boost::filesystem;

class invalid_file : std::exception {
  private:
    const char* message;
  public:
    invalid_file(const char* message) : message(message) {}

    const char* what() const throw() {
      return message;
    }
};

class level_file
{
  private:
    bool complete;
    int x, z;
    const fs::path path;
    
    // these must be public for the parser to be able to reach them.
    std::vector<light_marker> markers;
    
    boost::shared_ptr<nbt::ByteArray> blocks;
    boost::shared_ptr<nbt::ByteArray> skylight;
    boost::shared_ptr<nbt::ByteArray> heightmap;
    boost::shared_ptr<nbt::ByteArray> blocklight;
  public:
    level_file(const fs::path path);
    ~level_file();

    std::vector<light_marker> get_markers() {
      return markers;
    }
    
    /*
     * might throw invalid_file if the file is not gramatically correct
     */
    void read();
    
    bool is_read() {
      return complete;
    }
    
    boost::shared_ptr<nbt::ByteArray>
    get_blocks() {
      return blocks;
    }

    boost::shared_ptr<nbt::ByteArray>
    get_skylight() {
      return skylight;
    }
    
    boost::shared_ptr<nbt::ByteArray>
    get_heightmap() {
      return heightmap;
    }
    
    boost::shared_ptr<nbt::ByteArray>
    get_blocklight() {
      return blocklight;
    }
};

#endif /* _LEVEL_HPP_ */

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _LEVEL_H_
#define _LEVEL_H_

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

#include "nbt/nbt.hpp"

#include "global.hpp"
#include "blocks.hpp"
#include "marker.hpp"

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

namespace fs = boost::filesystem;

class level_file
{
  public:
    // these must be public for the parser to be able to reach them.
    bool islevel;
    bool grammar_error;
    size_t grammar_error_where;
    std::string grammar_error_why;
    bool ignore_blocks;
    bool in_te, in_sign;
    nbt::Int sign_x, sign_y, sign_z;
    std::string sign_text;
    std::vector<light_marker> markers;
    
    boost::scoped_ptr<nbt::ByteArray> blocks;
    boost::scoped_ptr<nbt::ByteArray> skylight;
    boost::scoped_ptr<nbt::ByteArray> heightmap;
    boost::scoped_ptr<nbt::ByteArray> blocklight;
    
    level_file(const fs::path path);
    ~level_file();
};

class fast_level_file
{
  public:
    // these must be public for the parser to be able to reach them.
    int xPos, zPos;
    bool has_xPos, has_zPos;
    bool islevel;
    bool grammar_error;
    size_t grammar_error_where;
    std::string grammar_error_why;
    const fs::path path;
    nbt::Parser<fast_level_file> parser;
    
    fast_level_file(const fs::path path, bool filename);
};

#endif /* _LEVEL_H_ */

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "global.h"

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

#include "nbt/nbt.h"

#include "global.h"
#include "color.h"
#include "image.h"
#include "blocks.h"
#include "marker.h"
#include "cache.h"

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace fs = boost::filesystem;

class level_file
{
  public:
    // these must be public for the parser to be able to reach them.
    bool islevel;
    bool grammar_error;
    bool from_cache;
    size_t grammar_error_where;
    std::string grammar_error_why;
    bool ignore_blocks;
    bool in_te, in_sign;
    nbt::Int sign_x, sign_y, sign_z;
    std::string sign_text;
    cache_file cache;
    bool cache_use;
    bool cache_hit;
    std::vector<light_marker> markers;
    
    boost::scoped_ptr<nbt::ByteArray> blocks;
    boost::scoped_ptr<nbt::ByteArray> skylight;
    boost::scoped_ptr<nbt::ByteArray> heightmap;
    boost::scoped_ptr<nbt::ByteArray> blocklight;
    boost::shared_ptr<image_operations> oper;
    
    level_file(settings_t& s);
    ~level_file();
    
    void load_file(const fs::path path);
    
    boost::shared_ptr<image_operations> get_image(settings_t& s);
    boost::shared_ptr<image_operations> get_oblique_image(settings_t& s);
    boost::shared_ptr<image_operations> get_obliqueangle_image(settings_t& s);
    boost::shared_ptr<image_operations> get_isometric_image(settings_t& s);
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

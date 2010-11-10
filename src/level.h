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
#include "image/color.hpp"
#include "blocks.h"
#include "marker.h"

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace fs = boost::filesystem;

class level_file
{
  protected:
    settings_t& s;
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
    boost::shared_ptr<image_operations> oper;
    
    level_file(settings_t& s);
    ~level_file();
    
    void load_file(const fs::path path);
    
    virtual boost::shared_ptr<image_operations> get_image() = 0;
};

class topdown_level_file : public level_file {
  public:
    topdown_level_file(settings_t& s) : level_file(s) {}
    boost::shared_ptr<image_operations> get_image();
};

class oblique_level_file : public level_file {
  public:
    oblique_level_file(settings_t& s) : level_file(s) {}
    boost::shared_ptr<image_operations> get_image();
};

class obliqueangle_level_file : public level_file {
  public:
    obliqueangle_level_file(settings_t& s) : level_file(s) {}
    boost::shared_ptr<image_operations> get_image();
};

class isometric_level_file : public level_file {
  public:
    isometric_level_file(settings_t& s) : level_file(s) {}
    boost::shared_ptr<image_operations> get_image();
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

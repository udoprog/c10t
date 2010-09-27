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

#include "nbt/nbt.h"

#include "color.h"
#include "image.h"
#include "blocks.h"

class level_file
{
  public:
    // these must be public for the parser to be able to reach them.
    nbt::ByteArray *blocks;
    nbt::ByteArray *skylight;
    nbt::ByteArray *heightmap;
    nbt::ByteArray *blocklight;
    int xPos;
    int zPos;
    bool islevel;
    bool grammar_error;
    size_t grammar_error_where;
    std::string grammar_error_why;
    std::string path;
    bool ignore_blocks;

    level_file(const char *path);
    ~level_file();

    image_buffer *get_image(settings_t& s);
    image_buffer *get_oblique_image(settings_t& s);
    image_buffer *get_obliqueangle_image(settings_t& s);
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
    std::string path;
    nbt::Parser<fast_level_file> parser;
    
    fast_level_file(const char *path);
};

#endif /* _LEVEL_H_ */

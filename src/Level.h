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

#include "Color.h"
#include "Image.h"
#include "blocks.h"


class Level
{
  public:
    // these must be public for the parser to be able to reach them.
    nbt::ByteArray *blocks;
    nbt::ByteArray *skylight;
    nbt::ByteArray *heightmap;
    nbt::ByteArray *blocklight;
    nbt::Int xPos;
    nbt::Int zPos;
    bool islevel;

    Level(const char *path);
    ~Level();
    
    Image *get_image(settings_t *s);
    Image *get_oblique_image(settings_t *s);
};

#endif /* _LEVEL_H_ */

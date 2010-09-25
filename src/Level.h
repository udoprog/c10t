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

#include <boost/detail/atomic_count.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "nbt/nbt.h"

#include "Color.h"
#include "Image.h"
#include "blocks.h"

bool parse_filename_coordinates(const std::string& path, int& x, int& z);
void transform_world_xz(int& x, int& z, int rotation);

class Level
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

    Level(settings_t& s, const std::string& path);
    ~Level();

    ImageBuffer *get_image(settings_t& s);
    ImageBuffer *get_oblique_image(settings_t& s);
    ImageBuffer *get_obliqueangle_image(settings_t& s);

    void load_data(settings_t& s);
    void unload_data();

  private:
    boost::mutex load_mutex;
    boost::detail::atomic_count load_count;
    bool load_tried;

    // Private and not implemented so it can't be used (avoids copies).
    Level(const Level& other);
};

#endif /* _LEVEL_H_ */

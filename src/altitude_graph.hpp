#ifndef STATISTICS_HPP
#define STATISTICS_HPP

// Include this first, to evade setjmp header bug
#include "image/format/png.hpp"

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <exception>

#include "image/format/png.hpp"

#include "global.hpp"
#include "players.hpp"

#include "image/image_base.hpp"
#include "image/memory_image.hpp"
#include "image/cached_image.hpp"
#include "image/algorithms.hpp"

#include "mc/world.hpp"
#include "mc/blocks.hpp"
#include "mc/utils.hpp"

#include "nbt/nbt.hpp"

using namespace std;
namespace fs = boost::filesystem;

class AltitudeGraph
{
public:
    AltitudeGraph(settings_t& _s);
    void createGraph();

    /* call this to register block information */
    void registerBloc(Byte value, int altitude);

    long getMax();
private:
    settings_t s;
    int width;
    int height;
    long altitudeRegistry[128];

};

#endif // STATISTICS_HPP

#ifndef STATISTICS_HPP
#define STATISTICS_HPP
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <exception>

#include "global.hpp"
#include "players.hpp"

#include "image/image_base.hpp"
#include "image/memory_image.hpp"
#include "image/cached_image.hpp"
#include "image/algorithms.hpp"
#include "image/format/png.hpp"

#include "mc/world.hpp"
#include "mc/blocks.hpp"
#include "mc/utils.hpp"

#include "nbt/nbt.hpp"

using namespace std;
namespace fs = boost::filesystem;

class BlocStatistics
{
public:
    BlocStatistics();
    void test();

    /* call this to register block information */
    void registerBloc(Byte value, Byte height);
private:

};

#endif // STATISTICS_HPP

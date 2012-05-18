// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

#include <sstream>
#include <string>
#include <list>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "config.hpp"

#include "nullstream.hpp"
#include "json.hpp"
#include "marker.hpp"

#include "image/memory_image.hpp"
#include "image/image_base.hpp"

#include "engine/engine_core.hpp"

#include "generate_map.hpp"
#include "generate_statistics.hpp"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "main_utils.hpp"

using namespace std;

namespace fs = boost::filesystem;

typedef uint64_t pos_t;

const uint8_t ERROR_BYTE = 0x01;
const uint8_t RENDER_BYTE = 0x10;
const uint8_t COMP_BYTE = 0x20;
const uint8_t IMAGE_BYTE = 0x30;
const uint8_t PARSE_BYTE = 0x40;
const uint8_t END_BYTE = 0xF0;

inline void cout_error(const string& message) {
  cout << hex << std::setw(2) << setfill('0') << static_cast<int>(ERROR_BYTE)
       << hex << message << flush;
}

inline void cout_end() {
  cout << hex << std::setw(2) << setfill('0') << static_cast<int>(END_BYTE) << flush;
}


int do_help(ostream& out) {
  out << "This program was made possible because of the work and inspiration by ZomBuster and Firemark" << endl;
  out << "" << endl;
  out << "Written by Udoprog et al." << endl;
  out << "" << endl;
  out << "The following libraries are in use for this program:" << endl
       << "  zlib (compression)"                  << endl
       << "    http://www.zlib.net"               << endl
       << "  boost (thread, filesystem)"          << endl
       << "    http://www.boost.org"              << endl
       << "  libpng (portable network graphics)"  << endl
       << "    http://www.libpng.org"             << endl
       << "  libfreetype (font loading)"          << endl
       << "    http://www.freetype.org"           << endl
       << "" << endl;
# if defined(C10T_DISABLE_THREADS)
  out << endl;
  out << "C10T_DISABLE_THREADS: Threads has been disabled for this build" << endl;
# endif
  out << endl;
  out << "Usage: c10t [options]" << endl;
  out << "Options:" << endl
       /*******************************************************************************/
    << "  -w, --world <world>       - use this world directory as input                " << endl
    << "  -o, --output <output>     - use this file as output file for generated png   " << endl
    << "  -S, --statistics <output> - create a statistics file of the entire world     " << endl
    << "      --graph-block <blockid>                                                  " << endl
    << "                            - make graph for block repartition by altitude     " << endl
    << "                              with filename <output>_graph.png               " << endl
    << endl
    << "  --log [file]              - Specify another location for logging warnings,   " << endl
    << "                              defaults to `c10t.log'                           " << endl
    << "  --no-log                  - Suppress logging of warnings                     " << endl
    << endl
    << "  -s, --silent              - execute silently, printing nothing except errors " << endl
    << "  -h, --help                - display this help text                           " << endl
    << "  -v, --version             - display version information                      " << endl
    << "  -D, --debug               - display debug information while executing        " << endl
    << "  -l, --list-colors         - list all available colors and block types        " << endl
    << endl
    << "Rendering options:" << endl
    << "  -q, --oblique             - Oblique rendering                                " << endl
    << "  -y, --oblique-angle       - Oblique angle rendering                          " << endl
    << "  -z, --isometric           - Isometric rendering                              " << endl
    << "  -Z, --fatiso              - A fat isometric rendering (very slow)            " << endl
    << "  -r <degrees>              - rotate the rendering 90, 180 or 270 degrees CW   " << endl
    << endl
    << "  -n, --night               - Night-time rendering mode                        " << endl
    << "  -H, --heightmap           - Heightmap rendering mode (black to white)        " << endl
    << endl
    << "Filtering options:" << endl
    << "  -e, --exclude <blockid>   - Exclude block-id from render (multiple occurences" << endl
    << "                              is possible)                                     " << endl
    << "  -i, --include <blockid>   - Include only this block-id in render (multiple   " << endl
    << "                              occurences is possible)                          " << endl
    << "  -a, --hide-all            - Show no blocks except those specified with '-i'  " << endl
    << "  -c, --cave-mode           - Cave mode - top down until solid block found,    " << endl
    << "                              then render bottom outlines only                 " << endl
    << "      --hell-mode           - Hell mode - top down until non-solid block found," << endl
    << "                              then render normally (a.k.a. nether)             " << endl
    << endl
    << "  -t, --top <int>           - Splice from the top, must be less than 128       " << endl
    << "  -b, --bottom <int>        - Splice from the bottom, must be greater than or  " << endl
    << "                              equal to zero.                                   " << endl
    << "  -L, --limits <int-list>   - Limit render to certain area. int-list form:     " << endl
    << "                              North,South,East,West, e.g.                      " << endl
    << "                              -L 0,100,-10,20 limiting between 0 and 100 in the" << endl
    << "                              north-south direction and between -10 and 20 in  " << endl
    << "                              the east-west direction.                         " << endl
    << "                              Note: South and West are the positive directions." << endl
    << "  -R, --radius <int>        - Limit render to a specific radius, useful when   " << endl
    << "                              your map is absurdly large and you want a 'fast' " << endl
    << "                              limiting option.                                 " << endl
    << "                              The default maximum radius is 1000 chunks.       " << endl
    << "      --center <x>,<z>      - Offset the map centering on limits by chunks <x> " << endl
    << "                              and <z>.                                         " << endl
    << endl
    << "  -N, --no-check            - Ignore missing <world>/level.dat                 " << endl
       /*******************************************************************************/
    << endl
    << "Performance options:" << endl
    << "  -M, --memory-limit <MB>   - Will limit the memory usage caching operations to" << endl
    << "                              file when necessary                              " << endl
    << "  -C, --swap-file <file>    - Swap file to use when memory limit `-M' is       " << endl
    << "                              reached, defaults to `swap.bin'                  " << endl
    << "  -m, --threads <int>       - Specify the amount of threads to use, for maximum" << endl
    << "                              efficency, this should match the amount of cores " << endl
    << "                              on your machine                                  " << endl
    << "      --prebuffer <int>     - Specify how many jobs to prebuffer for each      " << endl
    << "                              individual thread                                " << endl
    << "                                                                               " << endl
    << "  -B <set>                  - Specify the base color for a specific block id   " << endl
    << "                              <set> has the format <blockid>=<color>           " << endl
    << "                              <8 digit hex> specifies the RGBA values as       " << endl
    << "                              `<int>,<int>,<int>[,<int>]'. The side color will " << endl
    << "                              be a darkened variant of the base                " << endl
    << "                              example: `-B Grass=0,255,0,120'                  " << endl
    << "                              NOTE: Use only for experimentation, for a more   " << endl
    << "                                    permanent solution, use color palette files" << endl
    // this has been commented out since it is planned to be integrated for '-B' as a token scanning
    /*<< "  --side <set>              - Specify the side color for a specific block id   " << endl
    << "                              this uses the same format as '-B' only the color " << endl
    << "                              is applied to the side of the block              " << endl*/
    << "  -p, --split 'px1 px2 ..'  - Split the render into parts which must be pxX    " << endl
    << "                              pixels squared. `output' name must contain three " << endl
    << "                              format specifiers `%d' for `level' x and y       " << endl
    << "                              position. Supports multiple splits which will be " << endl
    << "                              placed on specific `level's.                     " << endl
    << "  --split-base dim          - Resize each tile generated by --split to `dim'   " << endl
    << "                              pixels squared (default: tiles are pxX pixels    " << endl
    << "                              squared)                                         " << endl
       /*******************************************************************************/
    << endl
    << "Other Options:" << endl
    << "  -x, --binary              - Will output progress information in binary form, " << endl
    << "                              good for integration with third party tools      " << endl
    << "  --require-all             - Will force c10t to require all chunks or fail    " << endl
    << "                              not ignoring bad chunks                          " << endl
    << "  --show-players[=NICKLIST] - Will draw out player position and names from the " << endl
    << "                              players database in <world>/players              " << endl
    << "                              it is possible to define which nicks to show by  " << endl
    << "                              specifying a comma separated list of nicks       " << endl
    << "  --show-signs[=PREFIX]     - Will draw out signs from all chunks, if PREFIX   " << endl
    << "                              is specified, only signs matching the prefix will" << endl
    << "                              be drawn                                         " << endl
    << "  --strip-sign-prefix       - When drawing sign text, removes the match prefix " << endl
    << "  --show-warps=<file>       - Will draw out warp positions from the specified  " << endl
    << "                              warps.txt file, as used by hey0's mod            " << endl
    << "  --show-coordinates        - Will draw out each chunks expected coordinates   " << endl
    << "  -P <file>                 - use <file> as palette, each line should take the " << endl
    << "                              form: <block-id> ' ' <color> ' ' <color>         " << endl
    << "  -W <file>                 - write the default color palette to <file>, this  " << endl
    << "                              is useful for figuring out how to write your own " << endl
    << "  --pedantic-broad-phase    - Will enforce that all level chunks are parsable  " << endl
    << "                              during broad phase by getting x/y/z positions    " << endl
    << "                              from a quick parsing                             " << endl
    << "  --no-alpha                - Set all colors alpha channel to opaque (solid)   " << endl
    << "  --striped-terrain         - Darken every other block on a vertical basis     " << endl
    << "                              which helps to distinguish heights               " << endl
    << "  --write-json <file>       - Write markers to <file> in JSON format instead of" << endl
    << "                              printing them on map                             " << endl
    << "  --write-js <file>         - Same as `write-json' with the exception that the " << endl
    << "                              result will be a valid javascript file containing" << endl
    << "                              a declaration for `var c10t_json'                " << endl
       /*******************************************************************************/
    << endl
    << "Font Options:" << endl
    << "  --ttf-path <font>         - Use the following ttf file when drawing text.    " << endl
    << "                              defaults to `font.ttf'                           " << endl
    << "  --ttf-size <size>         - Use the specified font size when drawing text.   " << endl
    << "                              defaults to `12'                                 " << endl
    << "  --ttf-color <color>       - Use the specified color when drawing text.       " << endl
    << "                              defaults to `0,0,0,255' (black)                  " << endl
    << "  --sign-color <color>      - Use the specified color when drawing signs.      " << endl
    << "                              defaults to <ttf-color>                          " << endl
    << "  --player-color <color>    - Use the specified color when showing players.    " << endl
    << "                              defaults to <ttf-color>                          " << endl
    << "  --warp-color <color>      - Use the specified color when showing warps.      " << endl
    << "                              defaults to <ttf-color>                          " << endl
    << "  --coordinate-color <color>                                                   " << endl
    << "                            - Use the specified color when drawing coordinates." << endl
    << "                              defaults to <ttf-color>                          " << endl
    << "  --cache-key <key>         - Indicates that c10t should cache operations using" << endl
    << "                              the unique cache key <key>, this should represent" << endl
    << "                              an unique combination of options. The cache files" << endl
    << "                              will be put in                                   " << endl
    << "                              <cache-dir>/<cache-key>/c.<coord>.cmap           " << endl
    << "  --cache-dir <dir>         - Use the following directory as cache directory   " << endl
    << "                              defaults to 'cache' if not specified             " << endl
    << "  --cache-compress          - Compress the cache files using zlib compression  " << endl
       /*******************************************************************************/
    << endl;
  out << endl;
  out << "Typical usage:" << endl;
  out << "    c10t -w /path/to/world -o /path/to/png.png" << endl;
  out << endl;
  out << "  Utilize render cache and apply a 256 MB memory restriction (rest will be written to image.dat):" << endl;
  out << "    c10t -w /path/to/world -o /path/to/png.png --cache-key='compressed' --cache-compress -M 256 -C image.dat" << endl;
  out << endl;
  out << "  Print out player positions using the font `example.ttf'" << endl;
  out << "    c10t -w /path/to/world -o /path/to/png.png --show-players --ttf-font example.ttf" << endl;
  out << endl;
  out << "  Split the result into multiple files, using 10 chunks across in each file, the two number formatters will be replaced with the x/z positions of the chunks" << endl;
  out << "    c10t -w /path/to/world -o /path/to/%d.%d.%d.png --split 10" << endl;
  out << endl;
  return 0;
}

int do_version(ostream& out) {
  out << "c10t - a cartography tool for minecraft" << endl;
# if defined(C10T_DISABLE_THREADS)
  out << endl;
  out << "C10T_DISABLE_THREADS: Threads have been disabled for this build" << endl;
  out << endl;
# endif
  out << "version: " << C10T_VERSION << ", built on " << __DATE__ << endl;
  out << "by: " << C10T_CONTACT << endl;
  out << "site: " << C10T_SITE << endl;
  return 0;
}

int do_colors(ostream& out) {
  out << "List of material Colors (total: " << mc::MaterialCount << ")" << endl;
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    out << i << ": " << mc::MaterialName[i] << " = " << mc::get_color(i) << endl;
  }
  
  return 0;
}

int main(int argc, char *argv[]){
    nullstream nil;
    ostream out(cout.rdbuf());
    ofstream out_log;

    vector<string> hints;
    
    out.precision(2);
    out.setf(ios_base::fixed);
    
    mc::initialize_constants();

    settings_t s;
    
    if (!read_opts(s, argc, argv)) {
        goto exit_error;
    }

    switch(s.action) {
    case Version:
        return do_version(out);
    case Help:
        return do_help(out);
    case ListColors:
        return do_colors(out);
    case WritePalette:
        if (!do_write_palette(s, s.palette_write_path)) {
            goto exit_error;
        }

        out << "Successfully wrote palette to " << s.palette_write_path << endl;
        return 0;
    case None:
        error << "No action specified, please type `c10t -h' for help";
        goto exit_error;
    default:
        break;
    }

    if (s.binary) {
        out.rdbuf(out_log.rdbuf());
    }

    if (s.silent) {
        out.rdbuf(nil.rdbuf());
    }

    if (!s.no_log) {
        out_log.open(path_string(s.output_log).c_str());
        out_log << "START LOG" << endl;
    }
    
    if (s.memory_limit_default) {
        hints.push_back("To use less memory, specify a memory limit with `-M <MB>', if it is reached c10t will swap to disk instead");
    }
    
    if (s.cache_use) {
        if (!fs::is_directory(s.cache_dir)) {
            error << "Directory required for caching: " << path_string(s.cache_dir);
            goto exit_error;
        }
        
        // then create the subdirectory using cache_key
        s.cache_dir = s.cache_dir / s.cache_key;
        
        if (!fs::is_directory(s.cache_dir)) {
            out << "Creating directory for caching: " << path_string(s.cache_dir) << endl;
            fs::create_directories(s.cache_dir);
        }
        
        {
            out << "Caching to directory: " << s.cache_dir << std::endl;
            out << "Cache compression: " << (s.cache_compress ? "ON" : "OFF")    << std::endl;
        }
    }
    
    if (!s.palette_read_path.empty()) {
        if (!do_read_palette(s, s.palette_read_path)) {
            goto exit_error;
        }

        out << "Sucessfully read palette from " << s.palette_read_path << endl;
    }
    
    if (s.world_path.empty())
    {
        error << "You must specify a world to render using `-w <directory>'";
        goto exit_error;
    }
    
    if (!s.nocheck)
    {
        fs::path level_dat = s.world_path / "level.dat";
        
        if (!fs::exists(level_dat)) {
            error << "Does not exist: " << path_string(level_dat);
            goto exit_error;
        }
    }
    
    /* hell mode requires entering the subdirectory DIM-1 */
    if (s.hellmode)
    {
        s.world_path = s.world_path / "DIM-1";
    }

    if (!fs::is_directory(s.world_path))
    {
        if (!fs::is_directory(s.world_path)) {
            error << "Does not exist: " << path_string(s.world_path);
            goto exit_error;
        }
    }
    
    switch(s.action) {
    case GenerateWorld:
        /* do some nice sanity checking prior to generating since this might
         * catch a couple of errors */

        if (s.output_path.empty()) {
            error << "You must specify output file using `-o <file>'";
            goto exit_error;
        }
        
        if (!fs::is_directory(s.output_path.parent_path())) {
            error << "Output directory does not exist: " << s.output_path;
            goto exit_error;
        }
        
        if (s.use_split) {
            try {
                boost::format(fs::basename(s.output_path)) % 0 % 0;
            } catch (boost::io::too_many_args& e) {
                error << "The `-o' parameter must contain two number format specifiers `%d' (x and y coordinates) - example: -o out/base.%d.%d.png";
                goto exit_error;
            }
        }

        if (!generate_map(out, out_log, error, s, hints, s.world_path, s.output_path)) {
            goto exit_error;
        }
        break;
    case GenerateStatistics:
        if (!generate_statistics(out, out_log, error, s, hints, s.world_path, s.statistics_path)) {
            goto exit_error;
        }
        break;
    default:
        error << "No action specified";
        goto exit_error;
    }
    
    if (hints.size() > 0 || warnings.size() > 0) {
        int i = 1;
        
        for (vector<std::string>::iterator it = warnings.begin(); it != warnings.end(); it++) {
            out << "WARNING " << i++ << ": " << *it << endl;
        }
        
        i = 1;
        for (vector<std::string>::iterator it = hints.begin(); it != hints.end(); it++) {
            out << "Hint " << i++ << ": " << *it << endl;
        }

        out << endl;
    }
    
    if (s.binary) {
        cout_end();
    }
    else {
        out << argv[0] << ": all done!" << endl;
    }
    
    mc::deinitialize_constants();
    
    if (!s.no_log) {
        out << "Log written to " << path_string(s.output_log) << endl;
        out_log << "END LOG" << endl;
        out_log.close();
    }
    
    return 0;
exit_error:
    if (s.binary) {
        cout_error(error.str());
    }
    else {
        out << argv[0] << ": " << error.str() << endl;
    }
    
    mc::deinitialize_constants();
    
    if (!s.no_log) {
        out << "Log written to " << path_string(s.output_log) << endl;
        out_log << "END LOG" << endl;
        out_log.close();
    }
    
    return 1;
}

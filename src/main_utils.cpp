#include "main_utils.hpp"

#include <getopt.h>

#include <sstream>
#include <fstream>
#include <iomanip>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "mc/blocks.hpp"

bool get_blockid(const std::string blockid_string, int& blockid);
bool parse_color(const std::string value, color& c);
bool parse_set(const char* set_str, int& blockid, color& c);
bool do_base_color_set(const char *set_str);
bool do_side_color_set(const char *set_str);

// Convert a string such as "-30,40,50,30" to the corresponding N,S,E,W integers,
// and fill in the min/max settings.
bool parse_limits(const std::string& limits_str, settings_t& s);
bool read_set(std::set<std::string>& set, const std::string s);

bool do_write_palette(settings_t& s, const boost::filesystem::path& path);
bool do_read_palette(settings_t& s, const boost::filesystem::path& path);

namespace fs = boost::filesystem;

using namespace std;
using namespace boost;

vector<std::string> hints;
vector<std::string> warnings;
stringstream error;

template<typename C>
void boost_split(C& collection, const string& s) {
    char_separator<char> sep(" \t\n\r,:");
    typedef tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(s, sep);

    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
        collection.insert(collection.end(), *tok_iter);
    }
}

bool get_blockid(const string blockid_string, int& blockid)
{
    for (int i = 0; i < mc::MaterialCount; i++) {
        if (blockid_string.compare(mc::MaterialName[i]) == 0) {
            blockid = i;
            return true;
        }
    }
    
    try {
        blockid = lexical_cast<int>(blockid_string);
    } catch(const bad_lexical_cast& e) {
        error << "Cannot be converted to number: " << blockid_string;
        return false;
    }
    
    if (!(blockid >= 0 && blockid < mc::MaterialCount)) {
        error << "Not a valid blockid: " << blockid_string;
        return false;
    }
    
    return true;
}

bool parse_color(const string value, color& c)
{
    std::vector<std::string> parts;

    boost_split(parts, value);

    int cr, cg, cb, ca=0xff;
    
    if (parts.size() < 3) {
        error << "Color sets must be of the form <red>,<green>,<blue>[,<alpha>] but was: " << value;
        return false;
    }

    try {
        cr = lexical_cast<int>(parts[0]);
        cg = lexical_cast<int>(parts[1]);
        cb = lexical_cast<int>(parts[2]);
        if (parts.size() == 4) {
            ca = lexical_cast<int>(parts[3]);
        }
    } catch(const bad_lexical_cast& e) {
        error << "Cannot be converted to a color: " << value;
        return false;
    }
    
    if (!(
            cr >= 0 && cr <= 0xff &&
            cg >= 0 && cg <= 0xff &&
            cb >= 0 && cb <= 0xff &&
            ca >= 0 && ca <= 0xff)) {
        error << "Color values must be between 0-255: " << value;
        return false;
    }
    
    c.r = color_i_to_f[cr];
    c.g = color_i_to_f[cg];
    c.b = color_i_to_f[cb];
    c.a = color_i_to_f[ca];
    
    return true;
}

bool parse_set(const char* set_str, int& blockid, color& c)
{
    istringstream iss(set_str);
    string key, value;
    
    assert(getline(iss, key, '='));
    assert(getline(iss, value));
    
    if (!get_blockid(key, blockid)) {
        return false;
    }

    if (!parse_color(value, c)) {
        return false;
    }
    
    return true;
}

bool do_base_color_set(const char *set_str) {
    int blockid;
    color c;
    
    if (!parse_set(set_str, blockid, c)) {
        return false;
    }

    mc::set_color(blockid, 0, c);
    return true;
}

bool do_side_color_set(const char *set_str) {
    int blockid;
    color c;
    
    if (!parse_set(set_str, blockid, c)) {
        return false;
    }

    if (mc::MaterialColorData[blockid].count > 0) {
        mc::MaterialColorData[blockid].side[0] = color(c);
    } else {
        error << "Side colors cannot be set (program flow error)" << endl;
    }
    return true;
}

// Convert a string such as "-30,40,50,30" to the corresponding N,S,E,W integers,
// and fill in the min/max settings.
bool parse_limits(const string& limits_str, settings_t& s) {
    std::vector<std::string> limits;

    boost_split(limits, limits_str);

    if (limits.size() != 4) {
        error << "Limit argument must of format: <N>,<S>,<E>,<W>";
        return false;
    }

    try {
        s.min_x = lexical_cast<int>(limits[0]);
        s.max_x = lexical_cast<int>(limits[1]);
        s.min_z = lexical_cast<int>(limits[2]);
        s.max_z = lexical_cast<int>(limits[3]);
    } catch(const bad_lexical_cast& e) {
        error << "Cannot be converted to set of numbers: " << limits_str;
        return false;
    }

    return true;
}

bool parse_tuple(const string& str, settings_t& s, int& a, int& b) {
    std::vector<std::string> parts;

    boost_split(parts, str);

    if (parts.size() != 2) {
        error << "Tuple must be of format: <num>,<num>";
        return false;
    }

    try {
        a = lexical_cast<int>(parts[0]);
        b = lexical_cast<int>(parts[1]);
    } catch(const bad_lexical_cast& e) {
        error << "Cannot be converted to set of numbers: " << str;
        return false;
    }

    return true;
}

bool read_set(std::set<string>& set, const string s) {
    boost_split(set, s);

    if (set.size() == 0) {
        error << "List must specify items separated by comma `,'";
        return false;
    }
    
    return true;
}

bool do_write_palette(settings_t& s, const fs::path& path) {
    std::ofstream pal(path.string().c_str());

    pal << "#" << left << setw(20) << "<block-id>" << setw(16) << "<base R,G,B,A>" << " " << setw(16) << "<side R,G,B,A>" << '\n';
    
    for (int i = 0; i < mc::MaterialCount; i++) {
        for (int j = 0; j < mc::MaterialColorData[i].count; j++) {
            color topCol = mc::MaterialColorData[i].top[j];
            color sideCol = mc::MaterialColorData[i].side[j];
            std::ostringstream name;
            name << mc::MaterialName[i] << ":" << j;
            pal << left << setw(20) << name.str() << " " << setw(16)
                << topCol << " " << setw(16) << sideCol << '\n';
        }
    }

    if (pal.fail()) {
        error << "Failed to write palette to " << path;
        return false;
    }
    
    return true;
}

bool do_read_palette(settings_t& s, const fs::path& path) {
    typedef tokenizer<boost::char_separator<char> > tokenizer;

    std::ifstream pal(path.string().c_str());
    char_separator<char> sep(" \t\n\r");
    
    while (!pal.eof()) {
        string line;
        getline(pal, line, '\n');
        
        tokenizer tokens(line, sep);
        
        int blockid = 0, i = 0, data = -1, dataPos = 0;
        color top, side;
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter, ++i) {
            string token = *tok_iter;
            
            if (token.at(0) == '#') {
                // rest is comment
                break;
            }
            
            switch(i) {
                case 0:
                    if ((size_t)(dataPos = token.find(':')) != string::npos) {
                        data = lexical_cast<int>(token.substr(dataPos + 1));
                        if (data < 0 || data >= 16) {
                            return false;
                        }
                        token.resize(dataPos);
                    }

                    if (!get_blockid(token, blockid)) {
                        return false;
                    }
                    break;
                case 1:
                    if (!parse_color(token, top)) {
                        return false;
                    }
                    
                    /* don't set color here so we only have ONE call to set_color below */
                    break;
                case 2:
                    if (!parse_color(token, side)) {
                        return false;
                    }
                    
	    /* colors read from the palette are unfortunately
                     * darkened by default and we can't avoid this for now :(
                     */
                    mc::set_color(blockid, data, top, side);
                    break;
                default:
                    break;
            }
        }
    }
    
    return true;
}

int flag;

struct option long_options[] =
 {
        {"world",                                                             required_argument,     0,             'w'},
        {"output",                                                            required_argument,     0,             'o'},
        {"top",                                                                 required_argument,     0,             't'},
        {"bottom",                                                            required_argument,     0,             'b'},
        {"limits",                                                            required_argument,     0,             'L'},
        {"radius",                                                            required_argument,     0,             'R'},
        {"memory-limit",                                                required_argument,     0,             'M'},
        {"cache-file",                                                    required_argument,     0,             'C'},
        {"swap-file",                                                     required_argument,     0,             'C'},
        {"exclude",                                                         required_argument,     0,             'e'},
        {"include",                                                         required_argument,     0,             'i'},
        {"rotate",                                                            required_argument,     0,             'r'},
        {"threads",                                                         required_argument,     0,             'm'},
        {"help",                                                                no_argument,                 0,             'h'},
        {"silent",                                                            no_argument,                 0,             's'},
        {"version",                                                         no_argument,                 0,             'v'},
        {"debug",                                                             no_argument,                 0,             'D'},
        {"list-colors",                                                 no_argument,                 0,             'l'},
        {"hide-all",                                                        no_argument,                 0,             'a'},
        {"no-check",                                                        no_argument,                 0,             'N'},
        {"oblique",                                                         no_argument,                 0,             'q'},
        {"oblique-angle",                                             no_argument,                 0,             'y'},
        {"isometric",                                                     no_argument,                 0,             'z'},
        {"fatiso",                                                            no_argument,                 0,             'Z'},
        {"cave-mode",                                                     no_argument,                 0,             'c'},
        {"night",                                                             no_argument,                 0,             'n'},
        {"heightmap",                                                     no_argument,                 0,             'H'},
        {"binary",                                                            no_argument,                 0,             'x'},
        {"require-all",                                                 no_argument,                 &flag,     0},
        {"show-players",                                                optional_argument,     &flag,     1},
        {"ttf-path",                                                        required_argument,     &flag,     2},
        {"ttf-size",                                                        required_argument,     &flag,     3},
        {"ttf-color",                                                     required_argument,     &flag,     4},
        {"show-coordinates",                                        no_argument,                 &flag,     5},
        {"pedantic-broad-phase",                                no_argument,                 &flag,     6},
        {"show-signs",                                                    optional_argument,     &flag,     7},
        {"sign-color",                                                    required_argument,     &flag,     8},
        {"player-color",                                                required_argument,     &flag,     9},
        {"coordinate-color",                                        required_argument,     &flag,     10},
        {"cache-key",                                                     required_argument,     &flag,     11},
        {"cache-dir",                                                     required_argument,     &flag,     12},
        {"cache-compress",                                            no_argument,                 &flag,     13},
        {"no-alpha",                                                        no_argument,                 &flag,     14},
        {"striped-terrain",                                         no_argument,                 &flag,     15},
        {"write-json",                                                    required_argument,     &flag,     16},
        {"write-js",                                                        required_argument,     &flag,     26},
        {"write-markers",                                             required_argument,     &flag,     21},
        {"split",                                                             required_argument,     0,             'p'},
        {"split-base",                                                    required_argument,     &flag,     27},
        {"show-warps",                                                    required_argument,     &flag,     18},
        {"warp-color",                                                    required_argument,     &flag,     19},
        {"prebuffer",                                                     required_argument,     &flag,     20},
        {"hell-mode",                                                     no_argument,                 &flag,     22},
        {"statistics",                                                    optional_argument,     0,             'S'},
        {"log",                                                                 required_argument,     &flag,     24},
        {"no-log",                                                            no_argument,                 &flag,     25},
        {"center",                                                            required_argument,     &flag,     30},
        {"graph-block",                                                 required_argument,     &flag,     31},
        {"strip-sign-prefix",                                     no_argument,                 &flag,     32},
        {"engine",                                                            required_argument,     &flag,     64},
        {0,                                                                         0,                                     0,             0}
};

bool read_opts(settings_t& s, int argc, char* argv[])
{
    int c;
    int blockid;
    int option_index;

    bool includes[mc::MaterialCount];
    bool excludes[mc::MaterialCount];

    for (int i = 0; i < mc::MaterialCount; i++) {
        includes[i] = false;
        excludes[i] = false;
    }

    bool exclude_all = false;

    while ((c = getopt_long(argc, argv, "DNvxcnHqzZyalshM:C:L:R:w:o:e:t:b:i:m:r:W:P:B:S:p:", long_options, &option_index)) != -1)
    {
        blockid = -1;
        
        if (c == 0) {
            switch (flag) {
            case 0:
                s.require_all = true;
                break;
            case 1:
                s.show_players = true;
                if (optarg != NULL) {
                    if (!read_set(s.show_players_set, optarg)) {
                        return false;
                    }
                }
                break;
            case 2:
                s.ttf_path = optarg;
                break;
            case 3:
                s.ttf_size = atoi(optarg);
                
                if (s.ttf_size <= 0) {
                    error << "ttf-size must be greater than 0";
                    return false;
                }
                
                break;
            case 4:
                if (!parse_color(optarg, s.ttf_color)) {
                    return false;
                }
                break;
            case 5:
                s.show_coordinates = true;
                break;
            case 6:
                s.pedantic_broad_phase = true;
                break;
            case 7:
                s.show_signs = true;

                if (optarg) {
                    s.show_signs_filter = optarg;
                    
                    if (s.show_signs_filter.empty()) {
                        error << "Sign filter must not be empty string";
                        return false;
                    }
                }
                
                break;
            case 8:
                if (!parse_color(optarg, s.sign_color)) {
                    return false;
                }
                
                s.has_sign_color = true;
                break;
            case 9:
                if (!parse_color(optarg, s.player_color)) {
                    return false;
                }
                
                s.has_player_color = true;
                break;
            case 10:
                if (!parse_color(optarg, s.coordinate_color)) {
                    return false;
                }
                
                s.has_coordinate_color = true;
                break;
            case 11:
                s.cache_use = true;
                s.cache_key = optarg;
                break;
            case 12:
                s.cache_dir = optarg;
                break;
            case 13:
                s.cache_compress = true;
                break;
            case 14:
                for (int i = mc::Air + 1; i < mc::MaterialCount; i++) {
                    for (int j = 0 ; j < mc::MaterialColorData[i].count ; j++) {
                        mc::MaterialColorData[i].top[j].a = 0xFF;
                        mc::MaterialColorData[i].side[j].a = 0xFF;
                    }
                }
                break;
            case 15: s.striped_terrain = true; break;
            case 21:
                hints.push_back("`--write-markers' has been deprecated in favour of `--write-json' - use that instead and note the new json structure");
            case 16:
                s.write_json = true;

                s.write_json_path = fs::system_complete(fs::path(optarg));
                
                {
                    fs::path parent = s.write_json_path.parent_path();
                    
                    if (!fs::is_directory(parent)) {
                        error << "Not a directory: " << parent.string();
                        return false;
                    }
                }
                
                break;
            case 26:
                s.write_js = true;

                s.write_js_path = fs::system_complete(fs::path(optarg));
                
                {
                    fs::path parent = s.write_js_path.parent_path();
                    
                    if (!fs::is_directory(parent)) {
                        error << "Not a directory: " << parent.string();
                        return false;
                    }
                }
                
                break;
            case 27:
                try {
                    s.split_base = boost::lexical_cast<int>(optarg);
                } catch(boost::bad_lexical_cast& e) {
                    error << "Cannot be converted to number: " << optarg;
                    return false;
                }

                if (!(s.split_base >= 1)) {
                    error << "split argument must be greater or equal to one";
                    return false;
                }
                break;
            case 18:
                s.show_warps = true;
                s.show_warps_path = fs::system_complete(fs::path(optarg));
                break;
            case 19:
                if (!parse_color(optarg, s.warp_color)) {
                    return false;
                }
                
                s.has_warp_color = true;
                break;
            case 20:
                s.prebuffer = atoi(optarg);
                
                if (s.prebuffer <= 0) {
                    error << "Number of prebuffered jobs must be more than 0";
                    return false;
                }
                
                break;
            case 22:
                s.hellmode = true;
                break;
            case 24:
                s.output_log = fs::system_complete(fs::path(optarg));
                break;
            case 25:
                s.no_log = true;
                break;
            case 30:
                if (!parse_tuple(optarg, s, s.center_x, s.center_z)) {
                    return false;
                }
                break;
            case 31:
                    try {
                        s.graph_block = boost::lexical_cast<int>(optarg);
                    } catch(boost::bad_lexical_cast& e) {
                        error << "Cannot be converted to number: " << optarg;
                        return false;
                    }
                if(!get_blockid(optarg, s.graph_block))
                {
                        return false;
                }
                break;

            case 32:
                s.strip_sign_prefix = true;
                break;
            case 64:
                s.engine_path = fs::system_complete(fs::path(optarg));

                if (!fs::is_regular_file(s.engine_path)) {
                    error << optarg << ": not a file";
                    return false;
                }

                s.engine_use = true;
                break;
            }
            
            continue;
        }
        
        switch (c)
        {
        case 'v':
            s.action = Version;
            break;
        case 'h':
            s.action = Help;
            break;
        case 'm':
            s.threads = atoi(optarg);
            
            if (s.threads <= 0) {
                error << "Number of worker threads must be more than 0";
                return false;
            }
            
            break;
        case 'q':
            s.mode = Oblique;
            break;
        case 'z':
            s.mode = Isometric;
            break;
        case 'Z':
            s.mode = FatIso;
            break;
        case 'D':
            s.debug = true;
            break;
        case 'y':
            s.mode = ObliqueAngle;
            break;
        case 'a':
            exclude_all = true;
            break;
        case 'i':
            if (!get_blockid(optarg, blockid)) return false;
            includes[blockid] = true;
            break;
        case 'e':
            if (!get_blockid(optarg, blockid)) return false;
            excludes[blockid] = true;
            break;
        case 'w':
            s.world_path = fs::system_complete(fs::path(optarg));
            break;
        case 'o':
            s.action = GenerateWorld;
            s.output_path = fs::system_complete(fs::path(optarg));
            break;
        case 's': 
            s.silent = true;
            break;
        case 'x':
            s.binary = true;
            break;
        case 'r':
            s.rotation = atoi(optarg) % 360;
            if (s.rotation < 0) {
                s.rotation += 360;
            }
            if (s.rotation % 90 != 0) {
                error << "Rotation must be a multiple of 90 degrees";
                return false;
            }

            break;
        case 'N': s.nocheck = true; break;
        case 'n': s.night = true; break;
        case 'H': s.heightmap = true; break;
        case 'c': s.cavemode = true; break;
        case 't':
            s.top = atoi(optarg);
            
            if (!(s.top > s.bottom && s.top < mc::MapY)) {
                error << "Top limit must be between `<bottom limit> - " << mc::MapY << "', not " << s.top;
                return false;
            }
            
            break;
        case 'L':
            if (!parse_limits(optarg, s)) {
                return false;
            }
            break;
        case 'R':
            s.max_radius = boost::lexical_cast<uint64_t>(optarg);
            
            if (s.max_radius < 1) {
                error << "Radius must be greater than zero";
                return false;
            }

            if (s.max_radius > 0xffffffff) {
                error << "Radius too big";
                return false;
            }
            break;
        case 'b':
            s.bottom = atoi(optarg);
            
            if (!(s.bottom < s.top && s.bottom >= 0)) {
                error << "Bottom limit must be between `0 - <top limit>', not " << s.bottom;
                return false;
            }
            
            break;
        case 'l':
            s.action = ListColors;
            break;
        case 'M':
            {
                s.memory_limit = boost::lexical_cast<int>(optarg);
            
                if (s.memory_limit <= 0) {
                    error << "Memory limit must be non-negative value, not " << s.memory_limit;
                    return false;
                }

                s.memory_limit_default = false;
            }
            break;
        case 'C':
            s.swap_file = fs::system_complete(fs::path(optarg));
            break;
        case 'W':
            s.palette_write_path = optarg;
            break;
        case 'P':
            s.palette_read_path = optarg;
            break;
        case 'B':
            if (!do_base_color_set(optarg)) return false;
            break;
        case 'S':
            s.action = GenerateStatistics;
            
            if (optarg != NULL) {
                s.statistics_path = fs::system_complete(fs::path(optarg));
            }
            break;
        case 'p':
            {
                std::list<std::string> result;
                std::string split_string(optarg);
                boost_split(result, split_string);

                BOOST_FOREACH(std::string str, result) {
                    unsigned int split_int = 0;

                    try {
                        split_int = boost::lexical_cast<int>(str);
                    } catch(boost::bad_lexical_cast& e) {
                        error << "Cannot be converted to number: " << str;
                        return false;
                    }

                    if (!(split_int >= 1)) {
                        error << "split argument must be greater or equal to one";
                        return false;
                    }

                    s.split.push_back(split_int);
                }
                    
                s.use_split = true;
            }
            break;
        case '?':
            if (optopt == 'c')
                error << "Option -" << optopt << " requires an argument";
            else if (isprint (optopt))
                error << "Unknown option `-" << optopt << "'";
            else
                error << "Unknown option character `\\x" << std::hex << static_cast<int>(optopt) << "'.";

             return false;
        default:
             error << "Unknown getopt error : ) - congrats, you broke it";
             return false;
        }
    }

    if (exclude_all) {
        for (int i = 0; i < mc::MaterialCount; i++)
        {
            s.excludes[i] = true;
        }
    }

    for (int i = 0; i < mc::MaterialCount; i++) {
        if (includes[i]) s.excludes[i] = false;
        if (excludes[i]) s.excludes[i] = true;
    }

    if (!s.palette_write_path.empty()) {
        s.action = WritePalette;
    }

    return true;
}

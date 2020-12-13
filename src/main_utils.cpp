#include "main_utils.hpp"

#include <getopt.h>

#include <unistd.h>

#include <sstream>
#include <fstream>
#include <iomanip>

#include <boost/property_tree/json_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "mc/blocks.hpp"

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
void boost_split(C& collection, const string& s)
{
    char_separator<char> sep(" \t\n\r,:");
    typedef tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(s, sep);

    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
        collection.insert(collection.end(), *tok_iter);
    }
}

bool get_blocktype(const std::string& block_string, mc::MaterialT*& block_type)
{
    // First attempt modern name lookup
    std::map<std::string, mc::MaterialT*>::iterator it = mc::MaterialMap.find(block_string);
    if (it == mc::MaterialMap.end()) {
        // Perhaps it is an old identifier then
        // TODO: also accept id:meta format
        try {
            int id = lexical_cast<int>(block_string);
            int meta = 0;
            boost::optional<mc::MaterialT*> material = mc::get_material_legacy(id, meta);
            if (material) {
                block_type = material.get();
                return true;
            }
        } catch(const bad_lexical_cast& e) {
            // ignore string to integer translation errors
        }
        error << "Block is nither a known block name nor a known numeric block identifier; " << block_string;
        return false;
    } else {
        block_type = it->second;
        return true;
    }
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

bool parse_set(const char* set_str, mc::MaterialT*& block_type, color& c)
{
    istringstream iss(set_str);
    string block_name, color_string;

    assert(getline(iss, block_name, '='));
    assert(getline(iss, color_string));

    if (!get_blocktype(block_name, block_type)) {
        return false;
    }

    if (!parse_color(color_string, c)) {
        return false;
    }

    return true;
}

bool do_base_color_set(const char *set_str)
{
    mc::MaterialT *material;
    color color_top;

    if (parse_set(set_str, material, color_top)) {
        material->top = color_top;
        return true;
    }
    return false;
}

bool do_side_color_set(const char *set_str)
{
    mc::MaterialT *material;
    color color_side;

    if (parse_set(set_str, material, color_side)) {
        material->side = color_side;
        return true;
    }
    return false;
}

// Convert a string such as "-30,40,50,30" to the corresponding N,S,E,W integers,
// and fill in the min/max settings.
bool parse_limits(const string& limits_str, settings_t& s)
{
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

bool parse_tuple(const string& str, settings_t& s, int& a, int& b)
{
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

bool read_set(std::set<string>& set, const string s)
{
    boost_split(set, s);

    if (set.size() == 0) {
        error << "List must specify items separated by comma `,'";
        return false;
    }

    return true;
}

bool do_write_palette(settings_t& s, const fs::path& path)
{
    std::ofstream pal(path.string().c_str());

    mc::MaterialMode_tr_to_string materialmode_tr;

    bool first_material = true;
    pal << "[" << std::endl;
    BOOST_FOREACH(mc::MaterialT &material, mc::MaterialTable) {
        if (!material.enabled) {
            continue;
        }
        if (first_material) {
            first_material = false;
        } else {
            pal << "," << std::endl;
        }
        pal << "  {" << std::endl;
        pal << "    \"namespace\": \"" << material.mc_namespace << "\"," << std::endl;
        pal << "    \"material\": \"" << material.name << "\"," << std::endl;
        pal << "    \"mode\": \"" << materialmode_tr.get_value(material.mode).get() << "\"," << std::endl;
        pal << "    \"top_color\": [" << material.top << "]," << std::endl;
        pal << "    \"side_color\": [" << material.side << "]," << std::endl;
        pal << "    \"darken\": false," << std::endl;
        if (material.legacy_ids.size() == 1) {
            pal << "    \"legacy_id\": " << material.legacy_ids[0] << "," << std::endl;
        } else if (material.legacy_ids.size() > 1) {
            pal << "    \"legacy_id\": [";
            bool first_id = true;
            BOOST_FOREACH(int i, material.legacy_ids) {
                if (first_id) {
                    first_id = false;
                } else {
                    pal << ", ";
                }
                pal << i;
            }
            pal << "]" << std::endl;
        }
        if (material.legacy_ids.size() > 0) {
            pal << "    \"legacy_meta\": " << material.legacy_meta << std::endl;
        }
        pal << "  }";
    }
    pal << "]" << std::endl;

    if (pal.fail()) {
        error << "Failed to write palette to " << path;
        return false;
    }

    return true;
}

int do_colors(std::ostream& out)
{
  out << "List of material Colors (total: " << mc::MaterialTable.size() << ")" << endl;

  BOOST_FOREACH(mc::MaterialT &material, mc::MaterialTable) {
    bool print_separator = false;
    BOOST_FOREACH(int i, material.legacy_ids) {
      if (print_separator) {
        out << ", ";
      }
      out << i << ":" << material.legacy_meta;
      print_separator = true;
    }
    if (print_separator) {
      out << "; ";
    }
    out << material.mc_namespace << ":" << material.name << " = top(" << material.top << "), side(" << material.side << ")" << endl;
  }

  return 0;
}

std::vector<int> int_vector_from_json(boost::property_tree::ptree parsed_json)
{
    std::vector<int> result = std::vector<int>();
    boost::optional<int> single = parsed_json.get_value_optional<int>();
    if (single) {
        result = { single.get() };
    } else {
        result.reserve(4);
        BOOST_FOREACH(boost::property_tree::ptree::value_type &entry, parsed_json) {
            single = entry.second.get_value_optional<int>();
            if (entry.first.empty() && single) {
                result.push_back(single.get());
            } else {
                error << "Error while parsing integer array" << std::endl;
            }
        }
        result.shrink_to_fit();
    }
    return result;
}

color color_from_json(const boost::property_tree::ptree color_json)
{
    boost::property_tree::ptree::const_iterator it = color_json.begin();
    if (it == color_json.end()) {
       throw "not a color";
    }
    int r = it->second.get_value<int>();
    it++;
    if (it == color_json.end()) {
       throw "not a color";
    }
    int g = it->second.get_value<int>();
    it++;
    if (it == color_json.end()) {
       throw "not a color";
    }
    int b = it->second.get_value<int>();
    it++;
    if (it == color_json.end()) {
       throw "not a color";
    }
    int a = it->second.get_value<int>();
    it++;
    if (it != color_json.end()) {
       throw "not a color";
    }
    return color(r, g, b, a);
}

struct color_translator
{
    typedef boost::property_tree::ptree internal_type;
    typedef color external_type;

    boost::optional<color> get_value(const boost::property_tree::ptree &pt) {
        try {
            return boost::optional<color>(color_from_json(pt));
        } catch(...) {
        }
        return boost::optional<color>();
    }
};

bool do_read_palette(settings_t& s, const fs::path& path)
{
    boost::property_tree::ptree parsed_json;
    try {
        std::ifstream json_src(path.string().c_str());
        boost::property_tree::read_json(json_src, parsed_json);
        json_src.close();
    } catch(boost::property_tree::json_parser::json_parser_error& e) {
        return false;
    }
    BOOST_FOREACH(boost::property_tree::ptree::value_type &mc_block_entry, parsed_json) {
      // flat JSON list expected, of which entries have no name (.first).
      boost::property_tree::ptree block_entry = mc_block_entry.second;
      boost::optional<std::string> mc_namespace = block_entry.get_optional<std::string>("namespace");
      boost::optional<std::string> material = block_entry.get_optional<std::string>("material");
      mc::MaterialMode_tr_from_string materialmode_tr;
      boost::optional<mc::MaterialMode> mode = block_entry.get_optional<mc::MaterialMode>("mode", materialmode_tr);
      color_translator color_tr;
      boost::optional<color> top_color = boost::optional<color>();
      if (boost::optional<boost::property_tree::ptree&> node = block_entry.get_child_optional("top_color")) {
        top_color = color_tr.get_value(node.get());
      }
      boost::optional<color> side_color = boost::optional<color>();
      if (boost::optional<boost::property_tree::ptree&> node = block_entry.get_child_optional("side_color")) {
        side_color = color_tr.get_value(node.get());
      }
      boost::optional<bool> darken_side = block_entry.get_optional<bool>("darken");
      std::vector<int> legacy_id;
      if (boost::optional<boost::property_tree::ptree&> node = block_entry.get_child_optional("legacy_id")) {
        legacy_id = int_vector_from_json(node.get());
      }
      boost::optional<int> legacy_meta = block_entry.get_optional<int>("legacy_meta");
      if (mc_namespace && material) {
        if (legacy_id.size() == 0 && legacy_meta) {
           error << "Invalid entry; " << mc_namespace.get() << ":" << material.get() << " has invalid legacy attributes." << std::endl;
        } else if(!mode) {
          error << "Invalid entry; " << mc_namespace.get() << ":" << material.get() << " is missing the material mode attribute." << std::endl;
        } else if (legacy_meta && (legacy_meta.get() < 0 || legacy_meta.get() >= 16)) {
          error << "Invalid entry; " << mc_namespace.get() << ":" << material.get() << " has an invalid legacy meta value, it must be >= 0 and < 16." << std::endl;
        } else if (mode.get() == mc::MaterialMode::LegacySlab && legacy_id.size() != 2) {
          error << "Invalid entry; " << mc_namespace.get() << ":" << material.get() << " has invalid legacy mode; exactly two legacy ids are required." << std::endl;
        } else {
          if (!top_color) {
            error << "Warning; block " << mc_namespace.get() << ":" << material.get() << " has no top color set" << std::endl;
          }
          color top = top_color.value_or(mc::SharedDefaultColor);
          color side = side_color.value_or(top);
          if (!darken_side || darken_side.get()) {
            side.darken(0x20);
          }
          mc::MaterialTable.push_back(mc::MaterialT { mc_namespace.get(), material.get(), mode.get(), top, side, legacy_id, legacy_meta.value_or(0), s.enable_all_blocks });
        }
      } else {
        error << "Invalid entry; entry is missing namespace or material attribute." << std::endl;
      }
    }

    // Whenever the MaterialTable is changed the palette lookup
    // tables needs to be regenerated.
    mc::reload_palette();

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
        {"side",                                                   required_argument,     &flag,     65},
        {0,                                                                         0,                                     0,             0}
};

bool read_opts(settings_t& s, int argc, char* argv[])
{
    int c;
    int option_index;

    while ((c = getopt_long(argc, argv, "DNvxcnHqzZyalshM:C:L:R:w:o:e:t:b:i:m:r:W:P:B:S:p:", long_options, &option_index)) != -1)
    {
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

                if (access(optarg, R_OK) == -1) {
                    error << "ttf path cannot be accessed";
                    return false;
                }

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
                s.disable_alpha = true;
                break;
            case 15:
                s.striped_terrain = true;
                break;
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
                s.graph_block = optarg;
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
            case 65:
                // TODO: this must be done after load
                if (!do_side_color_set(optarg)) return false;
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
            s.enable_all_blocks = false;
            break;
        case 'i':
            s.included.push_back(optarg);
            break;
        case 'e':
            s.excluded.push_back(optarg);
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
            // TODO: this must be done after load
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

    if (!s.palette_write_path.empty()) {
        s.action = WritePalette;
    }

    return true;
}

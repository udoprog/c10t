#ifndef __SETTINGS_T_HPP__
#define __SETTINGS_T_HPP__

#include <string>
#include <set>

#include <boost/filesystem.hpp>

#include "image/color.hpp"
#include "mc/utils.hpp"

enum mode {
  Top = 0x0,
  Oblique = 0x1,
  ObliqueAngle = 0x2,
  Isometric = 0x3,
  FatIso = 0x4
};

enum action {
  None,
  Version,
  Help,
  GenerateWorld,
  GenerateStatistics,
  ListColors,
  WritePalette
};

struct settings_t {
  settings_t(fs::path& install_path);

  bool binary;
  bool cache_compress;
  bool cache_use;
  bool cavemode;
  bool debug;
  bool has_coordinate_color;
  bool has_player_color;
  bool has_sign_color;
  bool has_warp_color;
  bool heightmap;
  bool hellmode;
  bool memory_limit_default;
  bool night;
  bool nocheck;
  bool no_log;
  bool pedantic_broad_phase;
  bool require_all;
  bool show_coordinates;
  bool show_players;
  bool show_signs;
  bool show_warps;
  bool silent;
  bool strip_sign_prefix;
  bool striped_terrain;
  bool use_split;
  bool write_js;
  bool write_json;
  bool disable_alpha;
  bool enable_all_blocks;
  std::list<std::string> included;
  std::list<std::string> excluded;
  std::list<std::string> top_color_overrides;
  std::list<std::string> side_color_overrides;
  color coordinate_color;
  color player_color;
  color sign_color;
  color ttf_color;
  color warp_color;
  enum mode mode;
  boost::filesystem::path install_path;
  boost::filesystem::path cache_dir;
  boost::filesystem::path output_log;
  boost::filesystem::path output_path;
  boost::filesystem::path palette_read_path;
  boost::filesystem::path palette_write_path;
  boost::filesystem::path show_warps_path;
  boost::filesystem::path statistics_path;
  boost::filesystem::path swap_file;
  boost::filesystem::path ttf_path;
  boost::filesystem::path world_path;
  boost::filesystem::path write_json_path;
  boost::filesystem::path write_js_path;
  boost::filesystem::path engine_path;
  bool engine_use;
  int bottom;
  uint64_t max_radius;
  int64_t min_x;
  int64_t max_x;
  int64_t min_z;
  int64_t max_z;
  int top;
  int ttf_size;
  size_t memory_limit;
  std::list<unsigned int> split;
  std::set<std::string> show_players_set;
  std::string cache_key;
  std::string show_signs_filter;
  // top/bottom used for slicing
  unsigned int prebuffer;
  unsigned int rotation;
  unsigned int split_base;
  unsigned int threads;

  std::string graph_block;

  int center_x;
  int center_z;

  enum action action;

  bool coord_out_of_range(mc::utils::level_coord& coord);
};

#endif /*__SETTINGS_T_HPP__*/

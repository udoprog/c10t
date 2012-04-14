#include "settings_t.hpp"

#include "mc/blocks.hpp"

settings_t::settings_t()
{
  this->excludes.reset(new bool[mc::MaterialCount]);
  
  for (int i = 0; i < mc::MaterialCount; i++) {
    this->excludes[i] = false;
  }

  this->excludes[mc::Air] = true;

  this->threads = 1;
  this->prebuffer = 4;
  
  this->split_base = 0;
  this->use_split = false;
  this->cavemode = false;
  this->hellmode = false;
  this->top = 127;
  this->bottom = 0;
  this->mode = Top;
  this->nocheck = false;
  this->silent = false;
  this->show_players = false;
  this->show_coordinates = false;
  this->show_signs = false;
  this->strip_sign_prefix = false;
  this->show_warps = false;
  this->require_all = false;
  this->striped_terrain = false;
  this->rotation = 0;
  this->binary = false;
  this->night = false;
  this->heightmap = false;
  this->debug = false;
  this->swap_file = "swap.bin";
  this->memory_limit = 1000;
  this->memory_limit_default = true;
  this->min_x = -10000;
  this->max_x = 10000;
  this->min_z = -10000;
  this->max_z = 10000;
  this->max_radius = 1000;
  this->ttf_path = fs::path("font.ttf");
  this->ttf_size = 12;
  this->ttf_color = color(0, 0, 0, 0xff);
  this->sign_color = color(0, 0, 0, 0xff);
  this->player_color = color(0, 0, 0, 0xff);
  this->has_player_color = false;
  this->has_sign_color = false;
  this->has_coordinate_color = false;
  this->has_warp_color = false;
  this->coordinate_color = color(0, 0, 0, 0xff);
  this->warp_color = color(0, 0, 0, 0xff);
  this->pedantic_broad_phase = false;
  this->cache_use = false;
  this->cache_key = "";
  this->cache_dir = "cache";
  this->cache_compress = false;
  this->write_json = false;
  this->write_js = false;
  this->no_log = false;
  this->output_log = fs::system_complete(fs::path("c10t.log"));
  this->output_path = fs::system_complete(fs::path("out.png"));
  this->statistics_path = fs::system_complete(fs::path("statistics.txt"));
  this->graph_block = -1;
  this->action = None;

  this->center_x = 0;
  this->center_z = 0;

  this->engine_use = false;
}

bool settings_t::coord_out_of_range(mc::utils::level_coord& coord)
{
  int x = coord.get_x() - center_x;
  int z = coord.get_z() - center_z;

  uint64_t x2 = uint64_t(x) * uint64_t(x);
  uint64_t z2 = uint64_t(z) * uint64_t(z);
  uint64_t r2 = max_radius * max_radius;
    
  return x < min_x
      || x > max_x
      || z < min_z
      || z > max_z
      || (x2 + z2) > r2+1;
}

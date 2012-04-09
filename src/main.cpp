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

#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/range/sub_range.hpp>

#include "config.hpp"

#include "image/format/png.hpp"
#include "image/image_base.hpp"
#include "image/memory_image.hpp"
#include "image/cached_image.hpp"
#include "image/algorithms.hpp"

#include "threads/renderer.hpp"
#include "2d/cube.hpp"

#include "global.hpp"
#include "cache.hpp"
#include "fileutils.hpp"
#include "players.hpp"
#include "text.hpp"
#include "json.hpp"
#include "altitude_graph.hpp"
#include "warps.hpp"
#include "selectors.hpp"

#include "mc/world.hpp"
#include "mc/blocks.hpp"
#include "mc/utils.hpp"
#include "mc/rotated_level_info.hpp"
#include "mc/level_info.hpp"
#include "mc/region_iterator.hpp"

#include "engine/engine_core.hpp"
#include "engine/topdown_engine.hpp"
#include "engine/oblique_engine.hpp"
#include "engine/obliqueangle_engine.hpp"
#include "engine/isometric_engine.hpp"
#include "engine/fatiso_engine.hpp"

#include "main_utils.hpp"
#include "dlopen.hpp"

using namespace std;
namespace fs = boost::filesystem;

struct nullstream: std::ostream {
  nullstream(): std::ios(0), std::ostream(0) {}
};

const uint8_t ERROR_BYTE = 0x01;
const uint8_t RENDER_BYTE = 0x10;
const uint8_t COMP_BYTE = 0x20;
const uint8_t IMAGE_BYTE = 0x30;
const uint8_t PARSE_BYTE = 0x40;
const uint8_t END_BYTE = 0xF0;

struct marker {
public:
  std::string text;
  std::string type;
  text::font_face font;
  int x, y, z;
  
  marker(std::string text, std::string type, text::font_face font, int x, int y, int z) :
      text(text), type(type), font(font), x(x), y(y), z(z)
  {  }
};

inline void cout_error(const string& message) {
  cout << hex << std::setw(2) << setfill('0') << static_cast<int>(ERROR_BYTE)
       << hex << message << flush;
}

inline void cout_end() {
  cout << hex << std::setw(2) << setfill('0') << static_cast<int>(END_BYTE) << flush;
}

/*
 * Store part of a level rendered as a small image.
 *
 * This will allow us to composite the entire image later and calculate sizes then.
 */
inline void populate_markers(settings_t& s, json::array* array, boost::shared_ptr<engine_core> engine, boost::ptr_vector<marker>& markers) {
  boost::ptr_vector<marker>::iterator it;
  
  for (it = markers.begin(); it != markers.end(); it++) {
    marker m = *it;
    
    mc::utils::level_coord coord = mc::utils::level_coord(m.x, m.z).rotate(s.rotation);
    
    pos_t x, y;
    
    engine->wp2pt(coord.get_x(), m.y, coord.get_z(), x, y);

    json::object* o = new json::object;
    
    o->put("text", new json::string(m.text));
    o->put("type", new json::string(m.type));
    
    // the projected coordinates
    o->put("x", new json::number(x));
    o->put("y", new json::number(y));
    
    // the real coordinates
    o->put("X", new json::number(m.x));
    o->put("Y", new json::number(m.y));
    o->put("Z", new json::number(m.z));
    
    array->push(o);
  }
  // don't bother to check for errors right now, but could be done using the "fail" accessor.
}

inline void overlay_markers(
    settings_t& s,
    image_ptr work_in_progress,
    boost::shared_ptr<engine_core> engine,
    boost::ptr_vector<marker>& markers
    )
{
  memory_image positionmark(5, 5);
  positionmark.fill(s.ttf_color);
  
  boost::ptr_vector<marker>::iterator it;
  
  for (it = markers.begin(); it != markers.end(); it++) {
    marker m = *it;

    if (!m.font.is_initialized()) {
      continue;
    }
    
    mc::utils::level_coord coord = mc::utils::level_coord(m.x, m.z).rotate(s.rotation);
    
    pos_t x, y;
    
    engine->wp2pt(coord.get_x(), m.y, coord.get_z(), x, y);
    
    m.font.draw(work_in_progress, m.text, x + 5, y);
    //all->safe_composite(x - 3, y - 3, positionmark);
  }
}

bool coord_out_of_range(settings_t& s, mc::utils::level_coord& coord)
{
  int x = coord.get_x() - s.center_x;
  int z = coord.get_z() - s.center_z;

  uint64_t x2 = uint64_t(x) * uint64_t(x);
  uint64_t z2 = uint64_t(z) * uint64_t(z);
  uint64_t r2 = s.max_radius * s.max_radius;
    
  return x < s.min_x
      || x > s.max_x
      || z < s.min_z
      || z > s.max_z
      || (x2 + z2) > r2+1;
}

template<typename T>
void out_dot(ostream& out, T total) {
  if ( (unsigned int) total == 0) out << " done!";
  else out << "." << flush;
}

void cout_uint_endl(ostream& out, unsigned int total) {
  out << " " << setw(8) << total << " parts" << endl;
}

void cout_uintpart_endl(ostream& out, unsigned int progress, unsigned int total) {
  out << " " << setw(8) << progress << " parts " << (progress * 100) / total << "%" << endl;
}

void cout_mb_endl(ostream& out, streampos progress, streampos total) {
  out << " " << setw(8) << fixed << float(progress) / 1000000 << " MB " << (progress * 100) / total << "%" << endl;
}

/**
 * Helper blocks
 */

/**
 * Load all warps from a database and push them to a container.
 */
template<typename T>
inline void load_warps(ostream& out, fs::path warps_path, T& warps)
{
  out << "warps: " << warps_path << ": " << flush;
  
  warps_db wdb(warps_path);
  
  try {
    wdb.read(warps);
    out << warps.size() << " warp(s) OK" << endl;
  } catch(warps_db_exception& e) {
    out << e.what() << endl;
  }
}

/**
 * Load all players from a database and push them to a container.
 */
template<typename T, typename S>
inline void load_players(ostream& out, fs::path show_players_path, T& players, S& player_set)
{
  out << "players: " << show_players_path << ": " << flush;
  
  players_db pdb(show_players_path, player_set);

  std::vector<player> all_players;
  
  try {
    pdb.read(all_players);
  } catch(players_db_exception& e) {
    out << " " << e.what() << endl;
    return;
  }

  out << all_players.size() << " player(s) found" << endl;

  BOOST_FOREACH(player p, all_players) {
    if (p.error) {
      out << "  " << p.path << ":" << p.error_where << ": " << p.error_why << endl;
      continue;
    }

    players.push_back(p);
  }

  out << "  " << players.size() << " player(s) OK" << endl;
}

/**
 * Push all players to a standard type of marker.
 */
template<typename P, typename T>
inline void push_player_markers(settings_t& s, text::font_face base_font, P& players, T& markers)
{
  text::font_face player_font = base_font;
  
  if (s.has_player_color) {
    player_font.set_color(s.player_color);
  }
  
  BOOST_FOREACH(player p, players) {
    if (p.zPos / mc::MapZ < s.min_z) continue;
    if (p.zPos / mc::MapZ > s.max_z) continue;
    if (p.xPos / mc::MapX < s.min_x) continue;
    if (p.xPos / mc::MapX > s.max_x) continue;
    
    markers.push_back(new marker(p.name, "player", player_font, p.xPos, p.yPos, p.zPos));
  }
}

/**
 * Push all signs to a standard type of marker.
 */
template<typename S, typename T>
inline void push_sign_markers(settings_t& s, text::font_face base_font, S& signs, T& markers)
{
  text::font_face sign_font = base_font;
  
  if (s.has_sign_color) {
    sign_font.set_color(s.sign_color);
  }
  
  BOOST_FOREACH(mc::marker lm, signs) {
    if (!s.show_signs_filter.empty() && lm.get_text().find(s.show_signs_filter) == string::npos) {
      continue;
    }
    
    if (!s.strip_sign_prefix) {
      markers.push_back(new marker(lm.get_text(), "sign", sign_font,
            lm.get_x(), lm.get_y(), lm.get_z()));
    } else {
      std::string text = lm.get_text().substr(s.show_signs_filter.size());
      markers.push_back(new marker(text, "sign", sign_font,
            lm.get_x(), lm.get_y(), lm.get_z()));
    }
  }
}

typedef std::map<mc::utils::level_coord, mc::rotated_level_info> levels_map;

/**
 * Push all coordinates to a standard type of marker.
 */
template<typename L, typename T>
inline void push_coordinate_markers(
    ostream& out,
    settings_t& s,
    text::font_face base_font,
    mc::world& world,
    L& levels,
    T& markers)
{
  text::font_face coordinate_font = base_font;
  
  if (s.has_coordinate_color) {
    coordinate_font.set_color(s.coordinate_color);
  }

  BOOST_FOREACH(levels_map::value_type value, levels)
  {
    mc::utils::level_coord c = value.second.get_coord();
    boost::shared_ptr<mc::level_info> l = value.second.get_level();
    
    if (c.get_z() - 4 < world.min_z) continue;
    if (c.get_z() + 4 > world.max_z) continue;
    if (c.get_x() - 4 < world.min_x) continue;
    if (c.get_x() + 4 > world.max_x) continue;
    if (c.get_z() % 10 != 0) continue;
    if (c.get_x() % 10 != 0) continue;
    std::stringstream ss;
    
    ss << "(" << l->get_x() * mc::MapX << ", " << l->get_z() * mc::MapZ << ")";
    if (s.debug) {
      out << "Pushing coordinate info " << ss.str() << endl;
    }
    markers.push_back(new marker(ss.str(), "coord", coordinate_font, c.get_x() * mc::MapX, 0, c.get_z() * mc::MapZ));
  }
}

/**
 * Push all warps to a standard type of marker.
 */
template<typename W, typename T>
inline void push_warp_markers(settings_t& s, text::font_face base_font, W& warps, T& markers)
{
  text::font_face warp_font = base_font;
  
  if (s.has_warp_color) {
    warp_font.set_color(s.warp_color);
  }
  
  /* initial code for projecting warps */
  BOOST_FOREACH(warp w, warps) { 
    if (w.zPos / mc::MapZ < s.min_z) continue;
    if (w.zPos / mc::MapZ > s.max_z) continue;
    if (w.xPos / mc::MapX < s.min_x) continue;
    if (w.xPos / mc::MapX > s.max_x) continue;
    
    marker *m = new marker(w.name, "warp", warp_font, w.xPos, w.yPos, w.zPos);
    markers.push_back(m);
  }
}

template<typename M>
void write_json_file(
    ostream& out,
    settings_t& s,
    boost::shared_ptr<engine_core> engine,
    mc::world& world,
    M& markers)
{
  // calculate world center
  engine_core::pos_t center_x, center_y;
  mc::utils::level_coord coord =
    mc::utils::level_coord(s.center_x*16, s.center_z*16).rotate(s.rotation);

  engine->wp2pt(coord.get_x(), 0, coord.get_z(), center_x, center_y);

  json::object file;
  json::object* json_static = new json::object;
  
  json_static->put("MapX", new json::number(mc::MapX));
  json_static->put("MapY", new json::number(mc::MapY));
  json_static->put("MapZ", new json::number(mc::MapZ));
  
  file.put("st", json_static);
  
  json::object* json_world = new json::object;
  
  json_world->put("cx", new json::number(center_x));
  json_world->put("cy", new json::number(center_y));
  json_world->put("dx", new json::number((world.diff_x + 1) * mc::MapX));
  json_world->put("dz", new json::number((world.diff_z + 1) * mc::MapZ));
  json_world->put("dy", new json::number(mc::MapY));
  json_world->put("mn_x", new json::number(world.min_x * 16));
  json_world->put("mn_z", new json::number(world.min_z * 16));
  json_world->put("mx_x", new json::number(world.max_x * 16));
  json_world->put("mx_z", new json::number(world.max_z * 16));
  json_world->put("rot", new json::number(s.rotation));
  json_world->put("mode", new json::number(s.mode));
  json_world->put("split_base", new json::number(s.split_base));
  json_world->put("split", new json::number(s.split.size()));
  
  file.put("world", json_world);
  
  json::array* markers_array = new json::array;
  populate_markers(s, markers_array, engine, markers);
  file.put("markers", markers_array);
  
  if (s.write_json) {
    out << "Writing json information: " << path_string(s.write_json_path) << endl;
    std::ofstream of(path_string(s.write_json_path).c_str());
    of << file;
    of.close();
  }

  if (s.write_js) {
    out << "Writing js (javascript `var c10t_json') information: " << path_string(s.write_js_path) << endl;
    std::ofstream of(path_string(s.write_js_path).c_str());
    of << "var c10t_json = " << file << ";";
    of.close();
  }
}

/**
 * Generate a map
 *
 * This is one of the main methods, it does the following steps.
 *
 * - Look for specificed databases.
 * - Scan the world for regions containing levels.
 * - Depending on settings, choose which rendering engine to use.
 * - Setup work-in-progress image to required type depending on predicted  memory
 *   use.
 * - Perform rendering phase where engine takes level information, and produces
 *   image_operations, composite all operations to the work-in-progress image.
 *   Try to distribute work evenly among threads.
 *
 */
bool generate_map(
    ostream& out,
    ostream& out_log,
    settings_t &s,
    fs::path& world_path,
    fs::path& output_path)
{
  out << endl << "Generating PNG Map" << endl << endl;
  
  // all marker source information.
  std::vector<player> players;
  std::vector<warp> warps;
  std::vector<mc::marker> signs;

  // this is where the actual markers will be populated later.
  boost::ptr_vector<marker> markers;
  
  // symbolic definition of a world.
  mc::world world(world_path);

  // where to store level info
  levels_map levels;
  
  // this is the rendering engine that will be used.
  boost::shared_ptr<engine_core> engine;
    
  // image to work against, could be backed by hard drive, or purely in memory.
  image_ptr work_in_progress;
  
  /**
   * Any of these options will trigger the database blocks to run.
   */
  bool use_any_database =
          s.show_players
       || s.show_signs
       || s.show_coordinates
       || s.show_warps;

  bool output_json = 
    s.write_json || s.write_js;

  /*
   * Look for specificed databases.
   */
  if (use_any_database)
  {
    out << " --- LOOKING FOR DATABASES --- " << endl;
    
    if (s.show_warps) {
      load_warps(out, s.show_warps_path, warps);
    }
    
    if (s.show_players) {
      load_players(out, world_path / "players", players, s.show_players_set);
    }

    if (s.show_signs) {
      out << "will look for signs in levels" << endl;
    }

    if (s.show_coordinates) {
      out << "will store chunk coordinates" << endl;
    }
  }
    
  {
    out << " --- SCANNING WORLD DIRECTORY --- " << endl;
    out << "world: " << path_string(world_path) << endl;
  }
  
  pallchunksel selector(new all_criterium_chunk_selector()); // selector;
  pchunksel in_range_pred (new predicate_criterium<in_range_predicate> (in_range_predicate(s)));
  
  selector->add_criterium(in_range_pred);
  if (s.selector != 0 ) { selector->add_criterium(s.selector) ;};

  panychunksel line_selector (new any_criterium_chunk_selector());
  if(s.lines_to_follow.size()>0){
        BOOST_FOREACH(std::list<point_surface> line_to_follow,s.lines_to_follow) {
		typedef boost::iterator_range< std::list<point_surface>::iterator > ilist;
		point_surface begin_line = line_to_follow.front(); 
		
		ilist points = boost::make_iterator_range(
			(line_to_follow.begin())++, 
			line_to_follow.end()
		);

		BOOST_FOREACH(point_surface end_line, points) {
			is_chunk_on_line is(begin_line,end_line);
			pchunksel p(new predicate_criterium<is_chunk_on_line>(is));
			line_selector->add_criterium(p);
			begin_line=end_line;
	  	}

		selector->add_criterium(line_selector);
		out << "added line criterium" << endl;
	}
  } 
  s.selector = selector; 
  /*
   * Scan the world for regions containing levels.
   */
  {
    nonstd::continious<unsigned int> reporter(out, 100, out_dot<unsigned int>, cout_uint_endl);
    mc::region_iterator iterator = world.get_iterator();

    int failed_regions = 0;
    int filtered_levels = 0;
    
    while (iterator.has_next()) {
      mc::region_ptr region = iterator.next();

      try {
        region->read_header();
      } catch(mc::bad_region& e) {
        ++failed_regions;
        out_log << path_string(region->get_path()) << ": could not read header" << std::endl;
        continue;
      }
      std::list<mc::utils::level_coord> coords;

      region->read_coords(coords);

      BOOST_FOREACH(mc::utils::level_coord c, coords) {
        mc::level_info::level_info_ptr level(new mc::level_info(region, c));
        mc::utils::level_coord coord = level->get_coord();

        if (! selector->select_level(coord)){
          ++filtered_levels;
          continue;
        }
        
        mc::rotated_level_info rlevel =
          mc::rotated_level_info(level, coord.rotate(s.rotation),coord);
        
        levels.insert( levels_map::value_type(rlevel.get_coord(), rlevel));

        world.update(rlevel.get_coord());
        reporter.add(1);
      }
    }
    
    reporter.done(0);

    if (failed_regions > 0) {
      out << "SEE LOG: " << failed_regions << " region(s) failed!" << endl;
    }

    if (filtered_levels > 0) {
      out << "SEE LOG: " << filtered_levels << " level(s) filtered!" << endl;
    }
  }

  if (levels.size() <= 0) {
    out << "No chunks to render" << endl;
    return 0;
  }
  
  if (s.debug) {
    out << " --- DEBUG WORLD INFO --- " << endl;
    out << "mc::world" << endl;
    out << "  min_x: " << world.min_x << endl;
    out << "  max_x: " << world.max_x << endl;
    out << "  min_z: " << world.min_z << endl;
    out << "  max_z: " << world.max_z << endl;
    out << "  levels: " << levels.size() << endl;
    out << "  radius: " << s.max_radius << endl;
    out << "  chunk pos: " << world.chunk_x << "x" << world.chunk_y << endl;
  }

  engine_settings engine_s;

  engine_s.rotation = s.rotation;
  engine_s.night = s.night;
  engine_s.heightmap = s.heightmap;
  engine_s.striped_terrain = s.striped_terrain;
  engine_s.hellmode = s.hellmode;
  engine_s.cavemode = s.cavemode;
  engine_s.top = s.top;
  engine_s.bottom = s.bottom;
  engine_s.excludes = s.excludes;
  engine_s.selector = s.selector;
 
  if (s.engine_use) {
    dl_t* dl = dl_open(path_string(s.engine_path).c_str());

    if (dl == NULL) {
      error << "Failed to open library: " << path_string(s.engine_path) << endl;
      return false;
    }

    typedef void (*hello_f)();

    //hello_f hello = (hello_f)dl_sym(dl, "hello");
    //hello();
    return true;
  }
  else {
    /**
     * Depending on settings, choose which rendering engine to use.
     */
    switch (s.mode) {
      case Top:
        engine.reset(new topdown_engine(engine_s, world));
        break;

      case Oblique:
        engine.reset(new oblique_engine(engine_s, world));
        break;

      case ObliqueAngle:
        engine.reset(new obliqueangle_engine(engine_s, world));
        break;

      case Isometric:
        engine.reset(new isometric_engine(engine_s, world));
        break;

      case FatIso:
        engine.reset(new fatiso_engine(engine_s, world));
        break;
    }
  }
  
  /**
   * Setup work-in-progress image to required type depending on predicted memory
   * use.
   */
  {
    pos_t image_width, image_height;
    pos_t level_width, level_height;
    
    engine->get_boundaries(image_width, image_height);
    engine->get_level_boundaries(level_width, level_height);
    
    pos_t memory_usage = (image_width * image_height * sizeof(color)) / 0x100000;
    
    if (memory_usage >= s.memory_limit) {
      {
        out << " --- BUILDING SWAP --- " << endl;
        out << "NOTE: A swap file is being built to accommodate high memory usage" << endl;
        out << "swap file: " << s.swap_file << endl;

        out << "swap size: " << memory_usage << " MB" << endl;
        out << "memory limit: " << s.memory_limit << " MB" << endl;
      }
      
      cached_image* image;
      
      try {
        image = new cached_image(s.swap_file, image_width, image_height, level_width, level_height);
      } catch(std::ios::failure& e) {
        if (errno != 0) {
          error << s.swap_file << ": " << strerror(errno);
        } else {
          error << s.swap_file << ": " << e.what() << ": could not open file";
        }
        
        return false;
      }
      
      work_in_progress.reset(image);

      nonstd::limited<streampos> c(out, 1024 * 1024, out_dot<streampos>, cout_mb_endl);
      
      try {
        image->build(c);
      } catch(std::ios::failure& e) {
        if (errno != 0) {
          error << s.swap_file << ": could not build cache: " << strerror(errno);
        } else {
          error << s.swap_file << ": could not build cache: " << e.what();
        }
        
        return false;
      }
    } else {
      {
        out << " --- ALLOCATING MEMORY --- " << endl;
        out << "memory usage: " << memory_usage << " MB" << endl;
        out << "memory limit: " << s.memory_limit << " MB" << endl;
      }
      
      work_in_progress.reset(new memory_image(image_width, image_height));
    }
  }

  /* reset image limits for cropping */
  engine->reset_image_limits();

  /**
   * Perform rendering phase where engine takes level information, and produces
   * image_operations, composite all operations to the work-in-progress image.
   * Try to distribute work evenly among threads.
   */
  {
    out << " --- RENDERING --- " << endl;

    unsigned int world_size = levels.size();
  
    renderer_settings renderer_s;

    renderer_s.cache_use = s.cache_use;
    renderer_s.cache_dir = s.cache_dir;
    renderer_s.cache_compress = s.cache_compress;

    int effective_threads = s.threads - 1;

    if (effective_threads <= 1) {
      effective_threads = 1;
    }

    renderer renderer(renderer_s, effective_threads, world_size);

    /**
     * The amount of currently enqueued jobs.
     */
    unsigned int queued = 0;

    /*
     * prebuffer is the amount of pending jobs that will be given available to
     * all threads.
     * This is done in steps to minimze memory usage, the size  of the steps steps
     * are decided by this parameter.
     */
    unsigned int prebuffer = effective_threads * s.prebuffer * 4;

    int cache_hits = 0;
    int failed_levels = 0;

    /**
     * Define a dynamically growing buffer to read regions in.
     * Is grown on demand, but never shrunk.
     */
    mc::dynamic_buffer region_buffer(mc::region::CHUNK_MAX);

    levels_map::iterator level_iter = levels.begin();
  
    nonstd::limited<unsigned int> reporter(out, 50, out_dot<unsigned int>, cout_uintpart_endl);
    reporter.set_limit(world_size);

    renderer.start();

    unsigned int prebuffer_limit = prebuffer / 2;
    uint32_t id = 1;

    while (level_iter != levels.end() || queued > 0) {
      if (queued < prebuffer_limit) {
        while (level_iter != levels.end() && queued < prebuffer) {
          levels_map::value_type value = *(level_iter++);

          mc::rotated_level_info rotated_level_info = value.second;
          mc::level_info_ptr level_info = rotated_level_info.get_level();

          mc::level_ptr level(new mc::level(level_info));

          try {
            level->read(region_buffer);
          } catch(mc::invalid_file& e) {
            out_log << level->get_path() << ": " << e.what() << endl;
            continue;
          }

          render_job job;

          job.engine = engine;
          job.order = id++;
          job.level = level;
          job.coord = rotated_level_info.get_coord();
          job.path = level_info->get_path();
	  job.nonrotated_coord = rotated_level_info.get_original_coord();

          renderer.give(job);
          ++queued;

          if (s.debug) {
            out << job.path << ": queued OK" << endl;
          }
        }
      }

      render_result p = renderer.get();
      --queued;

      if (s.debug) { out << p.level->get_path() << ": dequeued OK" << endl; }

      if (p.fatal) {
        reporter.add(1);
        out << p.level->get_path() << ": " << p.fatal_why << endl;
        continue;
      }

      if (p.cache_hit) {
        ++cache_hits;
      }
      
      if (p.signs.size() > 0) {
        if (s.debug) { out << "Found " << p.signs.size() << " signs"; };
        signs.insert(signs.end(), p.signs.begin(), p.signs.end());
      }

      reporter.add(1);

      try {
        pos_t x, y;
        engine->w2pt(p.coord.get_x(), p.coord.get_z(), x, y);

        // update image limits
        engine->update_image_limits(
            x + 1, y,
            x + p.operations->max_x,
            y + p.operations->max_y - 1);

        work_in_progress->composite(x, y, p.operations);
      } catch(std::ios::failure& e) {
        out << s.swap_file << ": " << strerror(errno);
        return false;
      }
    }
    
    reporter.done(0);

    if (failed_levels > 0) {
      out << "SEE LOG: " << failed_levels << " level(s) failed!" << endl;
    }
    
    if (s.cache_use) {
      out << "cache_hits: " << cache_hits << "/" << world_size << endl;
    }

    out << "image limits: "
        << engine->get_min_x() << "x" << engine->get_min_y() << " to "
        << engine->get_max_x() << "x" << engine->get_max_y() 
        << " will be the cropped image ("
        << (engine->get_max_x() - engine->get_min_x()) << "x"
        << (engine->get_max_y() - engine->get_min_y())
        << ")" << endl;

    image_ptr cropped = image::crop(work_in_progress, engine->get_min_x(), engine->get_max_x(), engine->get_min_y(), engine->get_max_y());
    work_in_progress = cropped;
  }
  
  if (use_any_database) {
    text::font_face font(s.ttf_path, s.ttf_size, s.ttf_color);
    
    /*
     * If we are only going to output json information, do not initialize font.
     * This will prevent any fonts from actually rendering anything later on.
     */
    if (!output_json) {
      try {
        font.init();
      } catch(text::text_error& e) {
        warnings.push_back(std::string("Failed to initialize font: ") + e.what());
      }
    }
    
    if (s.show_players) {
      push_player_markers(s, font, players, markers);
    }
    
    if (s.show_signs && signs.size() > 0) {
      push_sign_markers(s, font, signs, markers);
   }
    
    if (s.show_coordinates) {
      push_coordinate_markers(out, s, font, world, levels, markers);
    }
    
    if (s.show_warps) {
      push_warp_markers(s, font, warps, markers);
    }

  }
  
  if (output_json) {
    if (!use_any_database) {
      hints.push_back("Use `--write-json' in combination with `--show-*'"
          " in order to write different types of markers to file");
    }
    
    write_json_file(out, s, engine, world, markers);
  }
  else {
    overlay_markers(s, work_in_progress, engine, markers);
  }

  engine_core::pos_t center_x, center_y;
  engine->wp2pt(0, 0, 0, center_x, center_y);
  
  if (s.use_split) {
    out << " --- SAVING MULTIPLE IMAGES --- " << endl;

    int i = 0;

    image_ptr target;

    BOOST_FOREACH(unsigned int split_i, s.split) {
      if (!target && s.split_base > 0) {
        target.reset(new memory_image(s.split_base, s.split_base));
      }

      std::map<point2, image_base*> parts;

      image::split(work_in_progress, split_i, parts);

      out << "Level " << i << ": splitting into " << parts.size()
          << " image on " << split_i << "px" << endl;

      for (std::map<point2, image_base*>::iterator it = parts.begin(); it != parts.end(); it++) {
        const point2 p = it->first;
        image_ptr img(it->second);

        stringstream ss;
        ss << boost::format(path_string(output_path)) % i % p.x % p.y;
        fs::path path(ss.str());
        
        if (!fs::is_directory(path.parent_path())) {
          fs::create_directories(path.parent_path());
        }
        
        png_format::opt_type opts;
        
        opts.center_x = center_x;
        opts.center_y = center_y;
        opts.comment = C10T_COMMENT;
        
        std::string path_str(path_string(path));
        
        if (s.split_base > 0) {
          target->clear();
          img->resize(target);
        }
        else {
          target = img;
        }

        try {
          target->save<png_format>(path_str, opts);
        } catch (format_exception& e) {
          out << path_string(path) << ": " << e.what() << endl;
          continue;
        }

        out << path_string(path) << ": OK" << endl;
      }
      
      ++i;
    }
  }
  else {
    {
      out << " --- SAVING IMAGE --- " << endl;
      out << "path: " << path_string(output_path) << endl;
    }
    
    png_format::opt_type opts;
    
    opts.center_x = center_x;
    opts.center_y = center_y;
    opts.comment = C10T_COMMENT;
    
    try {
      work_in_progress->save<png_format>(path_string(output_path), opts);
    } catch (format_exception& e) {
      out << path_string(output_path) << ": " << e.what() << endl;
      return false;
    }
    
    out << path_string(output_path) << ": OK" << endl;
  }
  
  return true;
}

bool generate_statistics(
    std::ostream& out,
    std::ostream& out_log,
    settings_t &s,
    fs::path& world_path,
    fs::path& output_path)
{
    out << endl << "Generating Statistics File" << endl << endl;
    std::vector<player> players;
    mc::world world(world_path);

    AltitudeGraph *_stat = new AltitudeGraph(s);
    long statistics[mc::MaterialCount];

    for (int i = 0; i < mc::MaterialCount; i++) {
      statistics[i] = 0;
    }

    bool any_db =
      s.show_players
      || s.show_signs
      || s.show_coordinates
      || s.show_warps;

    if (any_db) {
      out << " --- LOOKING FOR DATABASES --- " << endl;

      if (s.show_players) {
        load_players(out, world_path / "players", players, s.show_players_set);
      }
    }

    int failed_regions = 0;
    int filtered_levels = 0;
    int failed_levels = 0;
    int levels = 0;

    {
      nonstd::continious<unsigned int> reporter(out, 100, out_dot<unsigned int>, cout_uint_endl);
      mc::region_iterator iterator = world.get_iterator();

      mc::dynamic_buffer region_buffer(mc::region::CHUNK_MAX);

      while (iterator.has_next()) {
        mc::region_ptr region = iterator.next();

        try {
          region->read_header();
        } catch(mc::bad_region& e) {
          ++failed_regions;
          out_log << region->get_path() << ": could not read header" << std::endl;
          continue;
        }

        std::list<mc::utils::level_coord> coords;

        region->read_coords(coords);

        BOOST_FOREACH(mc::utils::level_coord c, coords) {
          mc::level_info::level_info_ptr level(new mc::level_info(region, c));

          mc::utils::level_coord coord = level->get_coord();
          ++levels;

          if (coord_out_of_range(s, coord)) {
            ++filtered_levels;
            out_log << level->get_path() << ": (z,x) position"
                    << " (" << coord.get_z() << "," << coord.get_x() << ")"
                    << " out of limit" << std::endl;
            continue;
          }

          mc::level level_data(level);

          world.update(level->get_coord());

          try {
            level_data.read(region_buffer);
          } catch(mc::invalid_file& e) {
            ++failed_levels;
            out_log << level->get_path() << ": " << e.what();
            continue;
          }

          /*
          boost::shared_ptr<nbt::ByteArray> blocks = level_data.get_blocks();

          for (int i = 0; i < blocks->length; i++) {
            nbt::Byte block = blocks->values[i];
            statistics[block] += 1;
            if(s.graph_block > 0 && blocks->values[i] == s.graph_block)
            {
                // altitude is calculated as i % mc::MapY... Kind of messy, but...
                _stat->registerBloc(blocks->values[i], i % mc::MapY);
            }
          }
          */

          reporter.add(1);
        }
      }

      reporter.done(0);

      if (failed_regions > 0)
      {
        out << "SEE LOG: " << failed_regions << " region(s) failed!" << endl;
      }

      if (filtered_levels > 0)
      {
        out << "SEE LOG: " << filtered_levels << " level(s) filtered!" << endl;
      }

      if (failed_levels > 0)
      {
        out << "SEE LOG: " << failed_levels << " level(s) failed!" << endl;
      }
    }

    ofstream stats(path_string(output_path).c_str());

    stats << "[WORLD]" << endl;

    stats << "min_x " << world.min_x << endl;
    stats << "max_x " << world.max_x << endl;
    stats << "min_z " << world.min_z << endl;
    stats << "max_z " << world.max_z << endl;
    stats << "chunks " << (levels-filtered_levels-failed_levels)
                       << " of " << levels << endl;

    if (s.show_players) {
      stats << "[PLAYERS]" << endl;

      std::vector<player>::iterator plit = players.begin();

      for (; plit != players.end(); plit++) {
        player p = *plit;
        stats << p.name << " " << p.xPos << " " << p.yPos << " " << p.zPos << endl;
      }
    }

    stats << "[BLOCKS]" << endl;

    for (int i = 0; i < mc::MaterialCount; i++) {
      stats << setw(3) << i << " " << setw(24) << mc::MaterialName[i] << " " << statistics[i] << endl;
    }

    stats.close();

    if (stats.fail()) {
      error << "failed to write statistics to " << output_path;
      return false;
    }

    out << "statistics written to " << output_path;

    if(s.graph_block > 0)
        _stat->createGraph();

    return true;
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
    << "  -L, --limits <int-list>   - Limit render to certain area. int-list form      " << endl
    << "                              of chunk number in format                        " << endl
    << "                              North,South,East,West, e.g.                      " << endl
    << "                              -L 0,100,-10,20 limiting between 0 and 100 in the" << endl
    << "                              north-south direction and between -10 and 20 in  " << endl
    << "                              the east-west direction.                         " << endl
    << "                              Note: South and West are the positive directions." << endl
    << "  -Z, --limits <int-list>   - limt render to certain chunk along a polygon line" << endl
    << "                              area. int-list form                              " << endl
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
    default: break;
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
      out << "Cache compression: " << (s.cache_compress ? "ON" : "OFF")  << std::endl;
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
  
      if (!generate_map(out, out_log, s, s.world_path, s.output_path)) {
        goto exit_error;
      }
      break;
    case GenerateStatistics:
      if (!generate_statistics(out, out_log, s, s.world_path, s.statistics_path)) {
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

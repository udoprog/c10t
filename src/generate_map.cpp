#include "config.hpp"
#include "image/format/png.hpp"

#include "generate_map.hpp"

#include "text.hpp"
#include "players.hpp"
#include "warps.hpp"
#include "marker.hpp"

#include "algorithm.hpp"

#include "engine/engine_core.hpp"
#include "engine/topdown_engine.hpp"
#include "engine/oblique_engine.hpp"
#include "engine/obliqueangle_engine.hpp"
#include "engine/isometric_engine.hpp"
#include "engine/fatiso_engine.hpp"

#include "mc/blocks.hpp"
#include "mc/region_iterator.hpp"
#include "dlopen.hpp"

#include "image/algorithms.hpp"
#include "image/image_base.hpp"
#include "image/cached_image.hpp"
#include "image/memory_image.hpp"

#include "cache.hpp"

#include "json.hpp"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <ostream>
#include <iomanip>

using namespace std;
namespace fs = boost::filesystem;

typedef std::map<mc::utils::level_coord, mc::rotated_level_info> levels_map;

template<typename T>
void dot(std::ostream& out, T total)
{
  if (total == (T) 0x00)
  {
    out << " done!";
  }
  else
  {
    out << "." << std::flush;
  }
}

void parts_endl(std::ostream& out, unsigned int total)
{
  out << " " << setw(8) << total << " parts" << endl;
}

void parts_perc_endl(std::ostream& out, unsigned int progress, unsigned int total)
{
  out << " " << setw(8)
      << progress << " parts "
      << (progress * 100) / total << "%" << endl;
}

void mb_endl(std::ostream& out, streampos progress, streampos total)
{
  out << " " << setw(8)
      << fixed << float(progress) / 1000000 << " MB "
      << (progress * 100) / total << "%" << endl;
}

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
void push_player_markers(settings_t& s, text::font_face base_font, P& players, T& markers)
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
void push_sign_markers(settings_t& s, text::font_face base_font, S& signs, T& markers)
{
  text::font_face sign_font = base_font;

  if (s.has_sign_color) {
    sign_font.set_color(s.sign_color);
  }

  BOOST_FOREACH(mc::marker lm, signs) {
    if (!s.show_signs_filter.empty() && lm.get_text().find(s.show_signs_filter) == std::string::npos) {
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
void push_coordinate_markers(
    std::ostream& out,
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
    mc::level_info::level_info_ptr level_info = value.second.get_level();

    if (c.get_z() - 4 < world.min_z) continue;
    if (c.get_z() + 4 > world.max_z) continue;
    if (c.get_x() - 4 < world.min_x) continue;
    if (c.get_x() + 4 > world.max_x) continue;
    if (c.get_z() % 10 != 0) continue;
    if (c.get_x() % 10 != 0) continue;

    std::stringstream result;

    result << "(" << level_info->get_x() * mc::MapX
        << ", " << level_info->get_z() * mc::MapZ << ")";

    if (s.debug) {
      out << "Pushing coordinate info " << result.str() << std::endl;
    }

    markers.push_back(new marker(result.str(), "coord", coordinate_font, c.get_x() * mc::MapX, 0, c.get_z() * mc::MapZ));
  }
}

/**
 * Push all warps to a standard type of marker.
 */
template<typename W, typename T>
inline void push_warp_markers(
        settings_t& s,
        text::font_face base_font,
        W& warps,
        T& markers)
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

/*
 * Store part of a level rendered as a small image.
 *
 * This will allow us to composite the entire image later and calculate sizes then.
 */

void populate_markers(
        settings_t& s,
        json::array* array,
        boost::shared_ptr<engine_core> engine,
        boost::ptr_vector<marker>& markers)
{
  boost::ptr_vector<marker>::iterator it;

  for (it = markers.begin(); it != markers.end(); it++) {
    marker m = *it;

    mc::utils::level_coord original_coord(m.get_x(), m.get_z());
    mc::utils::level_coord coord = original_coord.rotate(s.rotation);

    pos_t x, y;

    engine->wp2pt(coord.get_x(), m.get_y(), coord.get_z(), x, y);

    json::object* o = new json::object;

    o->put("text", new json::string(m.get_text()));
    o->put("type", new json::string(m.get_type()));

    // the projected coordinates
    o->put("x", new json::number(x));
    o->put("y", new json::number(y));

    // the real coordinates
    o->put("X", new json::number(m.get_x()));
    o->put("Y", new json::number(m.get_y()));
    o->put("Z", new json::number(m.get_z()));

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

    text::font_face font = m.get_font();

    if (!font.is_initialized()) {
      continue;
    }

    mc::utils::level_coord original_coord(m.get_x(), m.get_z());
    mc::utils::level_coord coord = original_coord.rotate(s.rotation);

    pos_t x;
    pos_t y;

    engine->wp2pt(coord.get_x(), m.get_y(), coord.get_z(), x, y);

    font.draw(work_in_progress, m.get_text(), x + 5, y);
    //all->safe_composite(x - 3, y - 3, positionmark);
  }
}

/**
 * Helper blocks
 */

typedef std::map<mc::utils::level_coord, mc::rotated_level_info> levels_map;

void write_json_file(
    ostream& out,
    settings_t& s,
    boost::shared_ptr<engine_core> engine,
    mc::world& world,
    boost::ptr_vector<marker> markers)
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
    ostream& error,
    settings_t &s,
    std::vector<std::string>& hints,
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

      // Try both legacy and modern player dbs.
      fs::path full_path = world_path / "playerdata";
      if (!fs::is_directory(full_path)) {
        full_path = full_path = world_path / "players";
      }

      load_players(out, full_path, players, s.show_players_set);
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

  /*
   * Scan the world for regions containing levels.
   */
  {
    nonstd::continious<unsigned int> reporter(out, 100, dot, parts_endl);
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

        if (s.coord_out_of_range(coord)) {
          ++filtered_levels;
          out_log << level->get_path() << ": (z,x) position"
                  << " (" << coord.get_z() << "," << coord.get_x() << ")"
                  << " out of limit" << std::endl;
          continue;
        }

        mc::rotated_level_info rlevel =
          mc::rotated_level_info(level, coord.rotate(s.rotation));

        levels.insert(levels_map::value_type(rlevel.get_coord(), rlevel));

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

  if (s.engine_use) {
    dl_t* dl = dl_open(path_string(s.engine_path).c_str());

    if (dl == NULL) {
      error << "Failed to open library: " << path_string(s.engine_path) << endl;
      return false;
    }

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

      nonstd::limited<streampos> c(out, 1024 * 1024, image->get_size(), dot, mb_endl);

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

    int effective_threads = s.threads - 1;

    if (effective_threads <= 1) {
      effective_threads = 1;
    }

    int cache_hits = 0;
    int failed_levels = 0;

    /**
     * Define a dynamically growing buffer to read regions in.
     * Is grown on demand, but never shrunk.
     */
    mc::dynamic_buffer region_buffer(mc::region::CHUNK_MAX);

    nonstd::limited<unsigned int> reporter(out, 50, world_size, dot, parts_perc_endl);

    uint32_t id = 1;

    BOOST_FOREACH(levels_map::value_type value, levels) {
      reporter.add(1);

      mc::rotated_level_info rotated_level_info = value.second;
      mc::level_info_ptr level_info = rotated_level_info.get_level();

      mc::level_ptr level(new mc::level(level_info));
      fs::path path = level_info->get_path();

      try {
        level->read(region_buffer);
      } catch(mc::invalid_file& e) {
        out_log << path << ": " << e.what() << endl;
        continue;
      }

      mc::utils::level_coord coord = rotated_level_info.get_coord();

      image_operations_ptr operations(new image_operations(id++));

      bool cache_hit = false;
      time_t mod = level->modification_time();

      std::stringstream ss;
      ss << boost::format("%d.%d.cmap") % coord.get_x() % coord.get_z();
      std::string basename = ss.str();

      fs::path level_dir = mc::utils::level_dir(s.cache_dir, coord.get_x(), coord.get_z());
      cache_file cache(level_dir, basename, mod, s.cache_compress);

      if (s.cache_use) {
        if (cache.exists()) {
          if (cache.read(operations)) {
            cache_hit = true;
          }

          cache.clear();
        }
      }

      if (!cache_hit) {
        engine->render(level, operations);
      }

      //operations->optimize();

      if (s.cache_use) {
        // create the necessary directories required when caching
        cache.create_directories();

        // ignore failure while writing the operations to cache
        if (!cache.write(operations)) {
          // on failure, remove the cache file - this will prompt c10t to regenerate it next time
          cache.clear();
        }
      }

      if (s.debug) { out << path_string(path) << ": dequeued OK" << endl; }

      if (cache_hit) {
        ++cache_hits;
      }

      //if (p.signs.size() > 0) {
        //if (s.debug) { out << "Found " << p.signs.size() << " signs"; };
        //signs.insert(signs.end(), p.signs.begin(), p.signs.end());
      //}

      try {
        pos_t x, y;
        engine->w2pt(coord.get_x(), coord.get_z(), x, y);

        // update image limits
        engine->update_image_limits(
            x + 1, y,
            x + operations->max_x,
            y + operations->max_y - 1);

        work_in_progress->composite(x, y, operations);
      } catch(std::ios::failure& e) {
        out << path_string(s.swap_file) << ": " << strerror(errno);
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
        error << "failed to initialize font: " << e.what() << std::endl;
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

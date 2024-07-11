#include "altitude_graph.hpp"

#include "generate_statistics.hpp"

#include "mc/blocks.hpp"
#include "mc/region_iterator.hpp"
#include "mc/level_info.hpp"
#include "mc/level.hpp"

#include "players.hpp"
#include "main_utils.hpp"

#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>

using namespace std;

template<typename T>
void dot(std::ostream& out, T total)
{
  if (total == 0x00) {
    out << " done!";
  }
  else {
    out << "." << std::flush;
  }
}

void uint_endl(std::ostream& out, unsigned int total)
{
  out << " " << setw(8) << total << " parts" << endl;
}

bool generate_statistics(
    std::ostream& out,
    std::ostream& out_log,
    std::ostream& error,
    settings_t &s,
    std::vector<std::string>& hints,
    fs::path& world_path,
    fs::path& output_path)
{
    out << endl << "Generating Statistics File" << endl << endl;
    std::vector<player> players;
    mc::world world(world_path);

    AltitudeGraph *_stat = new AltitudeGraph(s);

    mc::MaterialT *materials = mc::MaterialTable.data();
    boost::shared_ptr<long[]> statistics(new long[mc::MaterialTable.size()]);

    for (size_t i = 0; i < mc::MaterialTable.size(); i++) {
      statistics[i] = 0;
    }

    boost::optional<mc::MaterialT*> graph_block;
    {
        mc::MaterialT *material;
        if(get_blocktype(s.graph_block, material)) {
            graph_block = boost::optional<mc::MaterialT*>(material);
        } else {
            graph_block = boost::optional<mc::MaterialT*>();
        }
    }

    bool any_db =
      s.show_players
      || s.show_signs
      || s.show_coordinates
      || s.show_warps;

    if (any_db) {
      out << " --- LOOKING FOR DATABASES --- " << endl;

      if (s.show_players) {
        error << "loading of players in altitiude graph has been disabled" << endl;
      }
      //if (s.show_players) {
        //load_players(out, world_path / "players", players, s.show_players_set);
      //}
    }

    int failed_regions = 0;
    int filtered_levels = 0;
    int failed_levels = 0;
    int levels = 0;

    {
      nonstd::continious<unsigned int> reporter(out, 100, dot, uint_endl);
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

          if (s.coord_out_of_range(coord)) {
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

          boost::shared_ptr<mc::Level_Compound> L = level_data.get_level();
          BOOST_FOREACH(boost::shared_ptr<mc::Section_Compound> Section, L->Sections) {
            for (int y = 15; y >= 0; y--) {
              int abs_y = (Section->get_y() * 16) + y;
               for (int z = 0; z < mc::MapZ; z++) {
                 for (int x = 0; x < mc::MapX; x++) {
                   mc::BlockT block;
                   if (Section->get_block(block, x, z, y)) {
                    if (block.material->enabled) {
                      size_t index = block.material - materials;
                      statistics[index] += 1;
                    }
                    if(graph_block && block.material == graph_block.get()) {
                      _stat->registerBloc(block.material, abs_y);
                    }
                  }
                }
              }
            }
          }

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
    for (size_t i = 0; i < mc::MaterialTable.size(); i++) {
      if (statistics[i] > 0) {
        std::string full_name = mc::MaterialTable[i].mc_namespace + ":" + mc::MaterialTable[i].name;
        stats << std::left << setw(32) << full_name << " " << statistics[i] << endl;
      }
    }

    stats.close();

    if (stats.fail()) {
      error << "failed to write statistics to " << output_path;
      return false;
    }

    out << "statistics written to " << output_path;

    if(graph_block)
        _stat->createGraph();

    return true;
}

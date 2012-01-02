#include "mc/level_info.hpp"
#include "mc/region.hpp"

namespace mc {
  level_info::level_info() : coord() {
  }

  level_info::level_info(region_ptr _region, int x, int z) : _region(_region), coord(x, z) {
  }

  level_info::level_info(region_ptr _region, utils::level_coord coord) : _region(_region) {
    utils::level_coord rc = utils::path_to_region_coord(_region->get_path());
    this->coord = utils::level_coord(rc.get_x() + coord.get_x(),
                                     rc.get_z() + coord.get_z());
  }

  std::string level_info::get_path() {
    std::stringstream ss;
    ss << _region->get_path().string() << "(" << coord.get_x() << "," << coord.get_z() << ")";
    return ss.str();
  }

  time_t level_info::modification_time() {
    return _region->read_modification_time(coord.get_x(), coord.get_z());
  }

  region_ptr level_info::get_region() {
    return _region;
  }

  bool level_info::operator<(const level_info& other) const {
    return coord < other.coord;
  }

  level_info level_info::rotate(int degrees) {
    return level_info(_region, coord.rotate(degrees));
  }

  int level_info::get_x() {
    return coord.get_x();
  }

  int level_info::get_z() {
    return coord.get_z();
  }

  const utils::level_coord level_info::get_coord() {
    return coord;
  }
}

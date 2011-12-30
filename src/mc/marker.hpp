#ifndef _MC_MARKER_HPP
#define _MC_MARKER_HPP

#include <string>

namespace mc {
  /*
   * designates a text related to a position
   * Possible usages:
   *   Signs
   *   Player Positions
   *   Custom Markers
   **/
  class marker {
  public:
    marker(std::string text, int x, int y, int z);
    std::string get_text();
    int get_x();
    int get_y();
    int get_z();
  private:
    std::string text;
    int x, y, z;
  };
}

#endif /* _MC_MARKER_HPP */

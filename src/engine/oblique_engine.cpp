#include "engine/oblique_engine.hpp"
#include "engine/block_rotation.hpp"
#include "engine/functions.hpp"

#include <boost/scoped_array.hpp>

void oblique_engine::render(level_ptr level, boost::shared_ptr<image_operations> oper, mc::utils::level_coord coord)
{
  pos_t iw, ih;

  project_limits(iw, ih);
  
  //const engine_settings& s = get_settings();
}

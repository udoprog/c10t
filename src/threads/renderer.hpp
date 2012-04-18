#ifndef __THREADS__RENDERER_HPP__
#define __THREADS__RENDERER_HPP__

#include "mc/utils.hpp"
#include "mc/world.hpp"
#include "mc/level.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <vector>

#include "cache.hpp"
#include "image/image_operations.hpp"
#include "engine/engine_core.hpp"

#include "threads/threadworker.hpp"
#include "threads/renderer_settings.hpp"

namespace fs = boost::filesystem;

struct render_result {
  boost::shared_ptr<mc::level> level;
  image_operations_ptr operations;
  bool fatal;
  std::string fatal_why;
  std::vector<mc::marker> signs;
  bool cache_hit;
  mc::utils::level_coord coord;
  fs::path path;
  
  render_result() : fatal(false), fatal_why("(no error)") {}

  bool operator<(const render_result& other) const {
    return coord < other.coord;
  }
};

struct render_job {
  int32_t order;
  boost::shared_ptr<mc::level> level;
  boost::shared_ptr<engine_core> engine;
  mc::utils::level_coord coord;
  fs::path path;
  mc::utils::level_coord nonrotated_coord;
};

class renderer : public threadworker<render_job, render_result> {
public:
  renderer_settings& r;
  
  renderer(renderer_settings r, int n, int total)
    : threadworker<render_job, render_result>(n, total), r(r) {
  }
  
  render_result work(render_job job) {
    render_result p;
    
    p.coord = job.coord;
    p.operations.reset(new image_operations(job.order));
    p.level = job.level;
    p.cache_hit = false;

    p.path = job.path;
    
    time_t mod = p.level->modification_time();
    std::stringstream ss;
    ss << boost::format("%d.%d.cmap") % job.coord.get_x() % job.coord.get_z();
    std::string basename = ss.str();

    cache_file cache(mc::utils::level_dir(r.cache_dir, job.coord.get_x(), job.coord.get_z()), basename, mod, r.cache_compress);
    
    if (r.cache_use) {
      if (cache.exists()) {
        if (cache.read(p.operations)) {
          p.cache_hit = true;
          return p;
        }
        
        cache.clear();
      }
    }
    
    job.engine->render(job.level, p.operations, job.nonrotated_coord);
   
    if (r.cache_use) {
      // create the necessary directories required when caching
      cache.create_directories();
      
      // ignore failure while writing the operations to cache
      if (!cache.write(p.operations)) {
        // on failure, remove the cache file - this will prompt c10t to regenerate it next time
        cache.clear();
      }
    }
    
    return p;
  }
};

#endif /* __THREADS__RENDERER_HPP__ */

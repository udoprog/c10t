#ifndef __THREADS__RENDERER_HPP__
#define __THREADS__RENDERER_HPP__

#include "mc/utils.hpp"
#include "mc/world.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include <vector>

#include "cache.hpp"
#include "image/image_operations.hpp"
#include "engine/engine_base.hpp"

#include "threads/threadworker.hpp"

namespace fs = boost::filesystem;

struct render_result {
  boost::shared_ptr<mc::level> level;
  image_operations_ptr operations;
  bool fatal;
  std::string fatal_why;
  std::vector<mc::marker> signs;
  bool cache_hit;
  mc::utils::level_coord coord;
  
  render_result() : fatal(false), fatal_why("(no error)") {}

  bool operator<(const render_result& other) const {
    return coord < other.coord;
  }
};

struct render_job {
  boost::shared_ptr<mc::level> level;
  boost::shared_ptr<engine_base> engine;
  mc::utils::level_coord coord;
};

class renderer : public threadworker<render_job, render_result> {
public:
  settings_t& s;
  
  renderer(settings_t& s, int n, int total) : threadworker<render_job, render_result>(n, total), s(s) {
  }
  
  render_result work(render_job job) {
    render_result p;
    
    p.coord = job.coord;
    p.operations.reset(new image_operations);
    p.level = job.level;
    p.cache_hit = false;

    /*p.path = job.path;
    
    cache_file cache(mc::utils::level_dir(s.cache_dir, job.xReal, job.zReal), p.path, s.cache_compress);
    
    if (s.cache_use) {
      if (cache.exists()) {
        if (cache.read(p.operations)) {
          p.cache_hit = true;
          return p;
        }
        
        cache.clear();
      }
    }*/
    
    p.signs = job.level->get_signs();
    job.engine->render(job.level, p.operations);
    
    /*if (s.cache_use) {
      // create the necessary directories required when caching
      cache.create_directories();
      
      // ignore failure while writing the operations to cache
      if (!cache.write(p.operations)) {
        // on failure, remove the cache file - this will prompt c10t to regenerate it next time
        cache.clear();
      }
    }*/
    
    return p;
  }
};

#endif /* __THREADS__RENDERER_HPP__ */

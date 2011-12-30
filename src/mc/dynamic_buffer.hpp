#ifndef _MC_DYNAMIC_BUFFER_HPP
#define _MC_DYNAMIC_BUFFER_HPP

#include <unistd.h>

namespace mc {
  /*
   * A dynamic buffer can be used when you need a fixed sized buffer that can
   * be expanded.
   *
   * This is useful if you have a shared buffer of optimal size which might
   * expand, but won't in the normal case.
   */
  class dynamic_buffer {
  public:
    enum {
      DEFAULT_FACTOR_MAX = 16
    };

    dynamic_buffer(size_t size);
    dynamic_buffer(size_t size, int factor_max);
    ~dynamic_buffer();

    size_t get_size();
    char* get();

    /**
     * Expand the buffer and return the amount it has been expanded with.
     */
    size_t expand();
  private:
    int factor;
    int factor_max;
    size_t size;
    size_t buffer_size;
    char* buffer;
  };
}

#endif /* _MC_DYNAMIC_BUFFER_HPP */

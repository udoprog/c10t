#include "mc/dynamic_buffer.hpp"

#include <string.h>

namespace mc {
  dynamic_buffer::dynamic_buffer(size_t size)
    : factor(1), factor_max(DEFAULT_FACTOR_MAX),
      size(size), buffer_size(size), buffer(new char[size])
  {
  }

  dynamic_buffer::dynamic_buffer(size_t size, int factor_max)
    : factor(1), factor_max(factor_max),
      size(size), buffer_size(size), buffer(new char[size])
  {
  }

  dynamic_buffer::~dynamic_buffer() {
    delete [] buffer;
  }

  size_t dynamic_buffer::get_size() {
    return buffer_size;
  }

  char* dynamic_buffer::get() {
    return buffer;
  }

  /**
   * Expand the buffer and return the amount it has been expanded with.
   */
  size_t dynamic_buffer::expand() {
    if (factor >= factor_max) {
      return 0;
    }

    factor += 1;

    size_t new_size = factor * size;
    char* new_buffer = new char[new_size];

    memcpy(new_buffer, buffer, buffer_size);
    delete[] buffer;

    size_t expanded_size = new_size - buffer_size;

    buffer = new_buffer;
    buffer_size = new_size;
    return expanded_size;
  }
}

#ifndef __NULLSTREAM_HPP__
#define __NULLSTREAM_HPP__

#include <ostream>
#include <iomanip>

struct nullstream : std::ostream {
  nullstream();
};

#endif /* __NULLSTREAM_HPP__ */

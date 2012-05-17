#include "nullstream.hpp"

nullstream::nullstream()
 : std::ios(0), std::ostream(0)
{
}

#ifndef _IMAGE_FORMAT_BASE_HPP_
#define _IMAGE_FORMAT_BASE_HPP_

#include <exception>

class format_exception : public std::exception
{
private:
  const char* why;
public:
  format_exception(const char* why) : why(why) { }
  
  const char* what() throw() {
    return this->why;
  }
};

#endif /* _IMAGE_FORMAT_BASE_HPP_ */

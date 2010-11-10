#ifndef _UTF8_H_
#define _UTF8_H_

#include <string>
#include <vector>
#include <stdint.h>
#include <ostream>

std::vector<uint32_t> utf8_decode(std::string);
std::string utf8_encode(std::vector<uint32_t>&);
void utf8_encode(uint32_t, std::ostream&);

#endif

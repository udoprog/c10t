/**
* This code is release into the public domain.
* If you find it useful i would like to be given a notice as the original author.
* Please contact me at: johnjohn.tedro@gmail.com
*/
#include "utf8.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <vector>
#include <sstream>
 
typedef struct utf8d_t {
  int i, w;
  unsigned long state;
  unsigned int pos;
} utf8d;
#define NUL 0x0
 
// mask out the last six bits of an octet and use as value.
// @i: the significance of the bits as a multiple of six
// @b: the actual octet
#define utf8d_trail_octet(i, b) (((b) & 0x3f) << (6 * (i)))
 
// Index the buffer using a specific stateholder.
// @u: the utf8_decoder struct
// @b: the write buffer
#define utf8d_current(u, b) *((u)->pos + (b))
 
// thread-safe coroutine statements
// uses a struct on the local heap space (specifically utf8_decoder)
#define COBegin(u) switch(u->state) { case 0:
#define COReturn(u, v) u->state = __LINE__;\
return (v);\
case __LINE__:
#define COEnd }

utf8d *
utf8d_new()
{
  utf8d *u = new utf8d;
  u->state = 0;
  u->i = 0;
  u->w = 0;
  u->pos = 0;
  return u;
}
 
/**
* @u: A pointer to the utf8_decoder struct
* @buffer: A write buffer with a minimum length of two
* @c: The next octet to use in the decoding stream.
*/
int utf8d_decode(utf8d *u, uint32_t *buffer, uint8_t c)
{
  COBegin(u);
  while (1) {
    if ((c >> 7) == 0x0) {
      utf8d_current(u, buffer) = c;
      u->i = 0;
    }
    // first octet says length two
    // that is 110xxxxx
    else if ((c >> 5) == (0x3 << 1)) {
      u->i = 1;
      utf8d_current(u, buffer) = utf8d_trail_octet(u->i, c & 0x1f);
    }
    // first octet says length three
    // that is 1110xxxx
    else if ((c >> 4) == (0x7 << 1)) {
      u->i = 2;
      utf8d_current(u, buffer) = utf8d_trail_octet(u->i, c & 0x0f);
    }
    // first octet says length four
    // that is 11110xxx
    else if ((c >> 3) == (0x0f << 1)) {
      u->i = 3;
      utf8d_current(u, buffer) = utf8d_trail_octet(u->i, c & 0x07);
    }
    // first octet says length five
    // that is 111110xx
    else if ((c >> 2) == (0x1f << 1)) {
      u->i = 4;
      utf8d_current(u, buffer) = utf8d_trail_octet(u->i, c & 0x03);
    }
    // first octet says length six
    // that is 1111110x
    else if ((c >> 1) == (0x3f << 1)) {
      u->i = 5;
      utf8d_current(u, buffer) = utf8d_trail_octet(u->i, c & 0x01);
    }
    // first octet does not signify unicode.
    else {
      goto error;
    }
 
    u->w = u->i;
 
    while (u->i-- > 0) {
      COReturn(u, 1);
      // decode failure part mismatch trailing octet is not 10xxxxxx.
      // where the xxxxxx parts signify payload.
      if ((c >> 6) != (0x1 << 1)) { goto reject; }
      
      // rfc states that no unicode character is longer than this.
      // code can obviously not be less than 0 either.
      //if (u->code > 0x10ffff || u->code < 0x0) { goto error; }
 
      // next part is an actual unicode part.
      utf8d_current(u, buffer) += utf8d_trail_octet(u->i, c);
    }
 
    // check for overly long sequence.
    switch (u->w) {
      case 1: if (utf8d_current(u, buffer) < 0x00000080) goto error; break;
      case 2: if (utf8d_current(u, buffer) < 0x00000800) goto error; break;
      case 3: if (utf8d_current(u, buffer) < 0x00010000) goto error; break;
      case 4: if (utf8d_current(u, buffer) < 0x00200000) goto error; break;
      case 5: if (utf8d_current(u, buffer) < 0x04000000) goto error; break;
    }
 
    // invalid utf-8 sequences.
    if (utf8d_current(u, buffer) >= 0xD800 && utf8d_current(u, buffer) <= 0xDFFF) goto error;
    if (utf8d_current(u, buffer) == 0xFFFE) goto error;
    if (utf8d_current(u, buffer) == 0xFFFF) goto error;
    
    u->pos++;
    COReturn(u, 1);
    continue;
error:
    utf8d_current(u, buffer) = NUL;
    u->pos++;
    COReturn(u, 1);
    continue;
reject:
    utf8d_current(u, buffer) = NUL;
    u->pos++;
    continue;
  }
 
  COEnd;
  return NUL;
}

#include <iostream>
 
std::vector<uint32_t> utf8_decode(std::string s) {
  std::vector<uint32_t> res;
  
  // this is the only requirement to get the coroutine working properly.
  utf8d *u = utf8d_new();
  
  // two character loockahead buffer.
  uint32_t buffer[2];
  uint32_t c;
  
  for (unsigned int i = 0; i < s.size(); i++) {
    utf8d_decode(u, buffer, s.at(i));
    
    // since we are using lookahead buffer,
    // only read when we have something.
    if (u->pos <= 0) {
      continue;
    }
 
    // read the number of characters we have.
    for (unsigned int r = 0; r < u->pos; r++) {
      c = buffer[r];
      
      if (c == NUL) {
        res.push_back('?');
        continue;
      }
      
      res.push_back(c);
    }
 
    u->pos = 0;
  }
  
  return res;
}

void utf8_encode(uint32_t v, std::ostream& out) {
  if (v <= 0x7F) {
    out << (char)(v & 0xff);
    return;
  }
  
  if (v <= 0x7FF) {
    uint8_t n1 = (v >> 8) & 0xFF;
    uint8_t n2 = v & 0xFF;
    out << (char)((0xC0 | (n1 << 2)) | ((n2 >> 6) & 0x03));
    out << (char)(0x80 | (n2 & 0x3F));
    return;
  }
}

#include <sstream>

std::string utf8_encode(std::vector<uint32_t>& v) {
  std::stringstream ss;
  
  for (std::vector<uint32_t>::iterator it = v.begin(); it != v.end(); it++) {
    utf8_encode(*it, ss);
  }
  
  return ss.str();
}

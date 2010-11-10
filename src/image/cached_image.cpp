#include "image/cached_image.hpp"

void image_operations::add_pixel(int x, int y, color &c)
{
  if (c.is_invisible()) {
    return;
  }
  
  if (!(x >= 0)) { return; }
  if (!(y >= 0)) { return; }
  if (!(x < maxx)) { return; }
  if (!(y < maxy)) { return; }
  
  image_operation oper;
  
  oper.x = (uint16_t)x;
  oper.y = (uint16_t)y;
  //oper.order = ++order;
  oper.c = c;
  
  if (!oper.c.is_transparent()) {
    size_t p = oper.x + oper.y * maxx;
    
    if (lookup[p]) {
      return;
    }
    
    lookup[p] = true;
  }
  
  operations.push_back(oper);
}

void image_operations::set_limits(int x, int y) 
{
  maxx = x;
  maxy = y;
  
  lookup.reset(new bool[maxx * maxy]);
  memset(lookup.get(), 0x0, sizeof(bool) * maxx * maxy);
  operations.reserve(maxx * maxy * 2);
}

// cached_image
void cached_image::set_pixel(size_t x, size_t y, color& c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  fs.seekp(get_offset(x, y), std::ios::beg);
  fs.write(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_pixel(size_t x, size_t y, color& c)
{
  if (!(x < get_width())) { return; }
  if (!(y < get_height())) { return; }
  fs.seekg(get_offset(x, y), std::ios::beg);
  fs.read(reinterpret_cast<char*>(&c), sizeof(color));
}

void cached_image::get_line(size_t y, size_t offset, size_t width, color* c)
{
  if (!(y < get_height())) { return; }
  if (!(offset < get_width())) { return; }
  if (!(width + offset < get_width())) { width = get_width() - offset; }
  fs.seekg(get_offset(0, y), std::ios::beg);
  fs.read(reinterpret_cast<char*>(c), sizeof(color) * width);
}

void cached_image::blend_pixel(size_t x, size_t y, color &c)
{
  // do nothing if color is invisible
  if (c.is_invisible()) {
    return;
  }
  
  size_t s = (x + y * get_width()) % buffer_size;
  
  icache* ic = &buffer[s];
  
  // cache hit
  if (ic->is_set()) {
    // cache hit, but wrong coordinates - flush pixel to file
    if (ic->x != x || ic->y != y)  {
      set_pixel(ic->x, ic->y, ic->c);
      ic->c = c;
      ic->x = x;
      ic->y = y;
      return;
    }
    
    ic->c.blend(c);
  }
  // cache miss - just set the cache
  else {
    ic->c = c;
    ic->x = x;
    ic->y = y;
  }
}


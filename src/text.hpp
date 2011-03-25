// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _TEXT_H_
#define _TEXT_H_

#include <exception>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H 

namespace fs = boost::filesystem;

#include "image/image_base.hpp"
#include "utf8.hpp"
#include <assert.h>

#include <iostream>

namespace text {
  class text_error : public std::exception {
  private:
    const std::string error;
  public:
    text_error(const std::string error) : error(error) {};

    virtual ~text_error() throw() {};
    
    virtual const char* what() const throw()
    {
      return error.c_str();
    }
  };

  class font_face {
  private:
    const fs::path font_path;
    int size;
    color base;
    bool initialized;
    FT_Library library;
    FT_Face face;
  public:
    font_face(const fs::path font_path, int size, color base);

    void init();

    void set_size(int size);
    
    void draw_bitmap(image_ptr image, FT_Bitmap* bitmap, pos_t pen_x, pos_t pen_y) const;
    
    void draw(image_ptr image, const std::string rawtext, int x, int y) const;

    void set_color(color& c);

    bool is_initialized();
  };
}

#endif 

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _TEXT_H_
#define _TEXT_H_

#include <exception>

#include <boost/filesystem.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H 

namespace fs = boost::filesystem;

#include "image.h"
#include "utf8.h"
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
    FT_Library library;
    FT_Face face;
    int size;
    color base;
  public:
    font_face(const fs::path font_path, int size, color base) throw(text_error) : font_path(font_path), base(base) {
      int error;
      
      error = FT_Init_FreeType( &library );

      if (error) {
        throw text_error("Failed to initialize the freetype2 library");
      }

      error = FT_New_Face(library, font_path.string().c_str(), 0, &face);

      if (error == FT_Err_Unknown_File_Format) {
        throw text_error("Could not open file - unknown file format: " + font_path.string());
      }
      else if (error) {
        throw text_error("Could not open file: " + font_path.string());
      }

      set_size(size);
    }

    void set_size(int size) {
      if (FT_Set_Pixel_Sizes(face, 0, size)) {
        throw text_error("Failed to set font resolution");
      }
      
      this->size = size;
    }
    
    void draw_bitmap(image_base& target, FT_Bitmap* bitmap, int pen_x, int pen_y) const {
      assert(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
      
      uint8_t* buffer = bitmap->buffer;
      
      for (int y = 0; y < bitmap->rows && y < target.get_height() + pen_y; y++) {
        for (int x = 0; x < bitmap->width && x < target.get_width() + pen_x; x++) {
          color c(base);
          c.a = buffer[x + y * bitmap->width];
          target.safe_blend_pixel(pen_x + x, pen_y + y, c);
        }
      }
    }
    
    void draw(image_base& image, const std::string rawtext, int x, int y) const {
      FT_GlyphSlot slot = face->glyph;

      int error;

      int pen_x = x, pen_y = y;
      
      std::vector<uint32_t> text = utf8_decode(rawtext);
      
      for (std::vector<uint32_t>::iterator it = text.begin(); it != text.end(); it++ ) {
        error = FT_Load_Char( face, *it, FT_LOAD_RENDER ); 
        if ( error ) continue;
        
        draw_bitmap(image, &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top);
        
        /* increment pen position */
        pen_x += slot->bitmap.width + 2;
      }
    }

    void set_color(color& c) {
      base = c;
    }
  };
}

#endif 

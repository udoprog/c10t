// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "text.hpp"
#include <unc/unc.hpp>

namespace text {
  font_face::font_face(const fs::path font_path, int size, color base)
      : font_path(font_path),
        size(size),
        base(base),
        initialized(false)
  {
  }

  void font_face::init()
  {
    int error;
    
    error = FT_Init_FreeType(&library);

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

    initialized = true;
  }

  void font_face::set_size(int size)
  {
    if (FT_Set_Pixel_Sizes(face, 0, size)) {
      throw text_error("Failed to set font resolution");
    }
    
    this->size = size;
  }
  
  void font_face::draw_bitmap(image_ptr image, FT_Bitmap* bitmap, pos_t pen_x, pos_t pen_y) const
  {
    assert(bitmap->pixel_mode == FT_PIXEL_MODE_GRAY);
    
    uint8_t* buffer = bitmap->buffer;

    pos_t s_bitmap_rows =
      boost::numeric_cast<pos_t>(bitmap->rows);
    
    pos_t s_bitmap_width =
      boost::numeric_cast<pos_t>(bitmap->width);
    
    for (pos_t y = 0; y < s_bitmap_rows && y < image->get_height() + pen_y; y++) {
      for (pos_t x = 0; x < s_bitmap_width && x < image->get_width() + pen_x; x++) {
        color c(base);
        c.a = color_i_to_f[buffer[x + y * bitmap->width]];
        image->safe_blend_pixel(pen_x + x, pen_y + y, c);
      }
    }
  }
  
  void font_face::draw(image_ptr image, const std::string rawtext, int x, int y) const
  {
    FT_GlyphSlot slot = face->glyph;

    int error;

    int pen_x = x, pen_y = y;
    
    unc::ustring text = unc::decode<unc::utf8>(rawtext);
    
    for (unc::ustring::iterator it = text.begin(); it != text.end(); it++ ) {
      uint32_t cc = *it;
      
      if (cc == '\n') {
        pen_x = x;
        pen_y += size + 2;
        continue;
      }
      
      error = FT_Load_Char( face, cc, FT_LOAD_RENDER ); 
      if ( error ) continue;
      
      draw_bitmap(image, &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top);
      
      /* increment pen position */
      pen_x += slot->bitmap.width + 2;
    }
  }

  void font_face::set_color(color& c)
  {
    base = c;
  }
  
  bool font_face::is_initialized()
  {
    return initialized;
  }
}

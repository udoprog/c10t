#ifndef FILEFORMAT_PNG
#define FILEFORMAT_PNG

#include <png.h>
#include <zlib.h>

#include "image/format/base.hpp"
#include "image/image_base.hpp"

#include <boost/lexical_cast.hpp>

struct png_config {
  std::string comment;
  size_t center_x, center_y;
};

class png_format {
  public:
    typedef png_config opt_type;

    static void save(image_base* image, const std::string& path, opt_type& opts)
    {
      FILE *fp;
      png_structp write_struct = NULL;
      png_infop info_struct = NULL;
      png_bytep bytes_row = NULL;
      color* color_row = NULL;

      fp = fopen(path.c_str(), "wb");

      if (fp == NULL)
      {
        throw format_exception(strerror(errno));
      }

      write_struct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

      if (write_struct == NULL)
      {
        fclose(fp);
        throw format_exception("failed to acquire write_struct");
      }

      /* optipng tends to select these options for c10t generated images */
      png_set_compression_level(write_struct, 9);
      png_set_compression_mem_level(write_struct, 8);
      png_set_compression_strategy(write_struct, Z_DEFAULT_STRATEGY);
      png_set_filter(write_struct, PNG_FILTER_TYPE_BASE, PNG_FILTER_NONE);

      info_struct = png_create_info_struct(write_struct);

      if (info_struct == NULL)
      {
        fclose(fp);
        png_destroy_write_struct(&write_struct, NULL);
        throw format_exception("failed to acquire info_struct");
      }

      /* this is where we end up on errors */
      if (setjmp(png_jmpbuf(write_struct)))
      {
        fclose(fp);
        png_destroy_write_struct(&write_struct, &info_struct);
        delete [] bytes_row;
        delete [] color_row;
        throw format_exception("unknown libpng error");
      }

      png_init_io(write_struct, fp);

      png_set_IHDR(write_struct, info_struct, image->get_width(), image->get_height(),
            8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      if (!opts.comment.empty())
      {
         png_text title_text;
         title_text.compression = PNG_TEXT_COMPRESSION_NONE;
         title_text.key = (char *)"Title";
         title_text.text = (char *)opts.comment.c_str();
         png_set_text(write_struct, info_struct, &title_text, 1);
      }

      {
         png_text title_text;
         title_text.compression = PNG_TEXT_COMPRESSION_NONE;
         title_text.key = (char *)"center-x";
         std::string text = boost::lexical_cast<std::string, int>(opts.center_x);
         title_text.text = const_cast<char*>(text.c_str());
         png_set_text(write_struct, info_struct, &title_text, 1);
      }
      
      {
         png_text title_text;
         title_text.compression = PNG_TEXT_COMPRESSION_NONE;
         title_text.key = (char *)"center-y";
         std::string text = boost::lexical_cast<std::string, int>(opts.center_y);
         title_text.text = const_cast<char*>(text.c_str());
         png_set_text(write_struct, info_struct, &title_text, 1);
      }

      png_write_info(write_struct, info_struct);

      bytes_row = new png_byte[4 * image->get_width()];
      color_row = new color[image->get_width()];

      for (size_t y = 0; y < image->get_height(); y++)
      {
        image->get_line(y, color_row);

        for (unsigned int i = 0; i < image->get_width(); i++)
        {
          bytes_row[i*4 + 0] = png_byte(color_row[i].r * 255.0f);
          bytes_row[i*4 + 1] = png_byte(color_row[i].g * 255.0f);
          bytes_row[i*4 + 2] = png_byte(color_row[i].b * 255.0f);
          bytes_row[i*4 + 3] = png_byte(color_row[i].a * 255.0f);
        }

        png_write_row(write_struct, bytes_row);
      }

      png_write_end(write_struct, NULL);

      fclose(fp);
      png_free_data(write_struct, info_struct, PNG_FREE_ALL, -1);
      png_destroy_write_struct(&write_struct, &info_struct);
      delete [] bytes_row;
      delete [] color_row;
    }
};

#endif /* FILEFORMAT_PNG */

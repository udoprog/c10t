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
      png_bytep row = NULL;
      
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
        free(row);
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

      row = (png_bytep) malloc(4 * image->get_width() * sizeof(png_byte));
      
      for (size_t y = 0; y < image->get_height(); y++)
      {
        //if (progress_c_cb != NULL) progress_c_cb(y, image->get_height());
        image->get_line(y, reinterpret_cast<color*>(row));
        png_write_row(write_struct, row);
      }
      
      //if (progress_c_cb != NULL) progress_c_cb(get_height(), get_height());
      
      png_write_end(write_struct, NULL);

      fclose(fp);
      png_free_data(write_struct, info_struct, PNG_FREE_ALL, -1);
      png_destroy_write_struct(&write_struct, &info_struct);
      free(row);
    }
};

#endif /* FILEFORMAT_PNG */

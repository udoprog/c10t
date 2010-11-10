#ifndef FILEFORMAT_PNG
#define FILEFORMAT_PNG

#include <png.h>

#include "image/image_base.hpp"

struct png_config {
  std::string comment;
};

class png_format {
  public:
    typedef png_config opt_type;
    
    static bool save(image_base* image, const std::string& path, opt_type& opts)
    {
      bool ret = true;
      
      FILE *fp;
      png_structp png_ptr = NULL;
      png_infop info_ptr = NULL;
      png_bytep row = NULL;
      
      fp = fopen(path.c_str(), "wb");
      
      if (fp == NULL)
      {
         ret = false;
         goto exit_cleanup;
      }
      
      png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      
      if (png_ptr == NULL)
      {
         ret = false;
         goto exit_cleanup;
      }

      info_ptr = png_create_info_struct(png_ptr);

      if (info_ptr == NULL)
      {
         ret = false;
         goto exit_cleanup;
      }

      if (setjmp(png_jmpbuf(png_ptr)))
      {
         ret = false;
         goto exit_cleanup;
      }

      png_init_io(png_ptr, fp);

      png_set_IHDR(png_ptr, info_ptr, image->get_width(), image->get_height(),
            8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
       
      if (!opts.comment.empty())
      {
         png_text title_text;
         title_text.compression = PNG_TEXT_COMPRESSION_NONE;
         title_text.key = (char *)"Title";
         title_text.text = (char *)opts.comment.c_str();
         png_set_text(png_ptr, info_ptr, &title_text, 1);
      }

      png_write_info(png_ptr, info_ptr);

      row = (png_bytep) malloc(4 * image->get_width() * sizeof(png_byte));
      
      for (size_t y = 0; y < image->get_height(); y++)
      {
        //if (progress_c_cb != NULL) progress_c_cb(y, image->get_height());
        image->get_line(y, reinterpret_cast<color*>(row));
        png_write_row(png_ptr, row);
      }
      
      //if (progress_c_cb != NULL) progress_c_cb(get_height(), get_height());
      
      png_write_end(png_ptr, NULL);

    exit_cleanup:
      if (fp != NULL)
      {
        fclose(fp);
      }
       
      if (info_ptr != NULL)
      {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
      }

      if (png_ptr != NULL)
      {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
      }

      if (row != NULL)
      {
        free(row);
      }
      
      return ret;
    }
};

#endif /* FILEFORMAT_PNG */

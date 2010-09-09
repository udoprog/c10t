#ifndef _IMG_H_
#define _IMG_H_

#include "Color.h"

class Image {
  private:
    int w;
    int h;
    
    Color *colors;
  
  public:
    Image(){
      w = 16;
      h = 16;
      colors = new Color[w*h];
      
      for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
          set_pixel(x, y, Color());
        }
      }
    }
    
    Image(int width, int height){
      w = width;
      h = height;
      colors = new Color[w*h];
      
      for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
          set_pixel(x, y, Color());
        }
      }
    }

    ~Image(){
      delete [] colors;
    }
    
    void set_pixel(int x, int y, Color q);
    Color get_pixel(int x, int y);
    int get_width();
    int get_height();
    
    /**
     * Composite one image ontop of this.
     */
    void composite(int xoffset, int yoffset, Image &img);
};

#endif /* _IMG_H_ */

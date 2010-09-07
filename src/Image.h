#ifndef _IMG_H_
#define _IMG_H_

#include "Color.h"

class Image {
  private:
    int w;
    int h;
  
  public:
    Color *d;

    Image(){
      w = 16;h = 16;
      d = new Color[w*h];
    }
    
    Image(int width, int height){
      w = width; h = height;
      d = new Color[w*h];
    }

    ~Image(){
      delete [] d;
    }

    void SetPixel(int x, int y, Color q);
    Color GetPixel(int x, int y);
    Color *GetPPointer(int x, int y);
};

#endif /* _IMG_H_ */

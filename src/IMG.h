#ifndef _IMG_H_
#define _IMG_H_

#include "Color.h"

class IMG {
  private:
    int w;
    int h;
  
  public:
    Color *d;

    IMG(){
      w = 16;h = 16;
      d = new Color[w*h];
    }

    IMG(int width,int height){
      w = width; h = height;
      d = new Color[w*h];
    }

    ~IMG(){
      delete [] d;
    }

    void SetPixel(int x, int y, Color q);
    Color GetPixel(int x, int y);
    Color *GetPPointer(int x, int y);
};

#endif /* _IMG_H_ */

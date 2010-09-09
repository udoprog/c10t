#ifndef _RENDER_H_
#define _RENDER_H_

#include "global.h"

#include "Image.h"

struct render{
private:
  Image *image;
  int x;
  int y;
public:
  render() : image(new Image(16,16)) {}
  
  ~render(){
    delete Q;
  }
};

#endif /* _RENDER_H_ */


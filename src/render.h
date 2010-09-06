#ifndef _RENDER_H_
#define _RENDER_H_

#include "global.h"

#include "IMG.h"

struct render{
  render() : Q(new IMG(16,16)) {}

  render(int bb){
  	if(bb == 69) {
  		Q = new IMG(16,144);
  	} else if(bb == 70) {
  		Q = new IMG(33,160);
  	} else{
  		Q = new IMG(16,16);
  	}

  	x = 0;
  	y= 0;
  }

  int x;
  int y;
  IMG *Q;
  ~render(){
  	delete Q;
  }
  bool isgood;
  
  int GetXc(){
    return y-x;
  }

  int GetYc(){
    return x+y;
  }

   bool operator < (const render& other)
  {
    if(slide == 70){
    int xx = (x)-(y); 
    int yy = (x)+(y);
    int xx2 = (other.x)-(other.y);
    int yy2 = (other.x)+(other.y);
    if(flip == 1 && Rotate == 1) return xx < xx2;
  if(flip == 1) return yy > yy2;
  if(Rotate == 1) return yy < yy2;
  return xx > xx2;


    }else{
if(flip == 1){
    if(Rotate == 0)
  return y > other.y;
    else
  return x > other.x;
    }

    else{
    if(Rotate == 0)
  return y < other.y;
    else
  return x < other.x;
    }

    }
  }

};

#endif /* _RENDER_H_ */


#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include "math.h"
#include <zlib.h>
#include <time.h>
#include <corona.h>

extern int flip;
extern int Rotate;
extern int cave;
extern int slide;


struct Color{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
	Color();
	Color(unsigned char rr ,unsigned char gg,unsigned char bb,unsigned char aa);
	~Color();
};


class IMG{
	public:
		Color * d;
		IMG();
		IMG(int width,int height);
		void SetPixel(int x,int y,Color q);
		Color GetPixel(int x,int y);
		Color * GetPPointer(int x,int y); 
		~IMG();
	private:
		int w;
		int h;
};

struct render{
	render();
	render(int bb);
	//IMG  i;
	int x;
	int y;
	//char rawx[4];
	IMG * Q;
	//char rawy[4];
	~render();
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
	if(Rotate == 1)	return yy < yy2;
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

Color Blend(Color A,Color B,int h);


unsigned long file_size(char *filename);



class Level{
public:
Level();
~Level();

void Save(char* name);

//read
int Read(int x,int y,int z,unsigned char * &d,int ret);
int GetHeight(int x,int y);

//edit
void Edit(int x,int y,int z,int block,unsigned char * &d); //edit one block

//Color Blend(Color A,Color B,int h);

//load data from chunk file
const render * LoadLevelFromFile(const char * name,const int slice,const bool CWATER,const int cut);

void MakeNewWorld(const wchar_t  * folder);
void WriteBuffer(const char * file);

bool IsBlock(int x,int y,int z);

const double getlight(int x,int y,int z,double sky,double block,bool CWATER,int slice);

Color GetColor(int blockid);


void AddName(const char * name);

void End();

std::string buffer;

int mapx;
int mapy;
int mapz;

int posx;
int posy;

unsigned int count[128];

private:

//char arrays for level data
unsigned char * blocklight;
unsigned char * skylight;
unsigned char * blocks;

Color BlockC[256];


unsigned char * databuffer;


};

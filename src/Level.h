#include "global.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <math.h>
#include <zlib.h>
#include <time.h>

#include "Color.h"
#include "render.h"
#include "IMG.h"

extern int flip;
extern int Rotate;
extern int daynight;
extern int cave;
extern int exclude;
extern int slide;

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
		const render *LoadLevelFromFile(settings_t *s, const char *name, const int slice, const bool CWATER, const int cut);

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

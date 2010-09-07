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
#include "Image.h"
#include "nbt/nbt.h"

extern int flip;
extern int Rotate;
extern int daynight;
extern int cave;
extern int exclude;
extern int slide;

Color Blend(Color A,Color B,int h);

unsigned long file_size(char *filename);

class Level{
  private:
    int mapsize;

    Color BlockC[256];

    unsigned char *databuffer;
  
  public:
    Level();
    ~Level();

    void Save(char* name);

    //read
    int Read(int x, int y, int z, nbt::Byte *d, int ret);
    int GetHeight(nbt::Byte *blocks, int x,int y);

    //edit
    void Edit(int x,int y,int z,int block, nbt::Byte* &d); //edit one block

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
};

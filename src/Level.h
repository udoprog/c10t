#ifndef _LEVEL_H_
#define _LEVEL_H_

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

#include "nbt/nbt.h"

#include "Color.h"
#include "Image.h"
#include "blocks.h"

////blockcolors[21] = color(222,50,50,0x0);
////blockcolors[22] = color(222,136,50,0x0);
////blockcolors[23] = color(222,222,50,0x0);
////blockcolors[24] = color(136,222,50,0x0);
////blockcolors[25] = color(50,222,50,0x0);
////blockcolors[26] = color(50,222,136,0x0);
////blockcolors[27] = color(50,222,222,0x0);
////blockcolors[28] = color(104,163,222,0x0);
////blockcolors[29] = color(120,120,222,0x0);
////blockcolors[30] = color(136,50,222,0x0);
////blockcolors[31] = color(174,74,222,0x0);
////blockcolors[32] = color(222,50,222,0x0);
////blockcolors[33] = color(222,50,136,0x0);
////blockcolors[34] = color(77,77,77,0x0);
//blockcolors[35] = color(222,222,222,0x0); //color(143,143,143,0x0); 
////blockcolors[36] = color(222,222,222,0x0);
//blockcolors[37] = color(255,0,0,0x0);
//blockcolors[38] = color(255,255,0,0);
//blockcolors[41] = color(232,245,46,0x0);
//blockcolors[42] = color(191,191,191,0x0);
//blockcolors[43] = color(200,200,200,0x0);
//blockcolors[44] = color(200,200,200,0x0);
//blockcolors[45] = color(170,86,62,0x0);
//blockcolors[46] = color(160,83,65,0x0);
//blockcolors[48] = color(115,115,115,0x0);
//blockcolors[49] = color(26,11,43,0x0);
//blockcolors[50] = color(245,220,50,200);
//blockcolors[51] = color(255,170,30,200);
////blockcolors[52] = color(245,220,50,0x0); unnecessary afaik
//blockcolors[53] = color(157,128,79,0x0);
//blockcolors[54] = Color(125,91,38,0x0);
////blockcolors[55] = Color(245,220,50,0x0); unnecessary afaik
//blockcolors[56] = Color(129,140,143,0x0);
//blockcolors[57] = Color(45,166,152,0x0);
//blockcolors[58] = Color(114,88,56,0x0);
//blockcolors[59] = Color(146,192,0,0x0);
//blockcolors[60] = Color(95,58,30,0x0);
//blockcolors[61] = Color(96,96,96,0x0);
//blockcolors[62] = Color(96,96,96,0x0);
//blockcolors[63] = Color(111,91,54,0x0);
//blockcolors[64] = Color(136,109,67,0x0);
//blockcolors[65] = Color(181,140,64,32);
//blockcolors[66] = Color(150,134,102,180);
//blockcolors[67] = Color(115,115,115,0x0);
//blockcolors[71] = Color(191,191,191,0x0);
//blockcolors[73] = Color(131,107,107,0x0);
//blockcolors[74] = Color(131,107,107,0x0);
//blockcolors[75] = Color(181,140,64,32);
//blockcolors[76] = Color(255,0,0,200);
//blockcolors[78] = Color(255,255,255,0x0);
//blockcolors[79] = Color(83,113,163,51);
//blockcolors[80] = Color(250,250,250,0x0);
//blockcolors[81] = Color(25,120,25,0x0);
//blockcolors[82] = Color(151,157,169,0x0);
//blockcolors[83] = Color(193,234,150,255);

static Color blockcolors[] = {
  // Air
  Color(255,255,255,0),
  // Stone
  Color(120,120,120,255),
  // Grass
  Color(117,176,73,255),
  // Dirt
  Color(134,96,67,255),
  // Cobblestone
  Color(115,115,115,255),
  // Wood
  Color(157,128,79,255),
  // Sapling
  Color(120,120,120,0),
  // Bedrock
  Color(84,84,84,255),
  // Water
  Color(38,92,255,51),
  // StationaryWater
  Color(38,92,255,51),
  // Lava
  Color(255,90,0,255),
  // StationaryLava
  Color(255,90,0,255),
  // Sand
  Color(218,210,158,255),
  // Gravel
  Color(136,126,126,255),
  // GoldOre
  Color(143,140,125,255),
  // IronOre
  Color(136,130,127,255),
  // CoalOre
  Color(115,115,115,255),
  // Log
  Color(102,81,51,255),
  // Leaves
  Color(60,192,41,100),
  // Sponge
  Color(),
  // Glass
  Color(255,255,255,64),
  // Cloths
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  Color(),
  //YellowFlower
  Color(),
  //RedRose
  Color(),
  //BrownMushroom
  Color(),
  //RedMushroom
  Color(),
  //GoldBlock
  Color(),
  //IronBlock
  Color(),
  //DoubleStep
  Color(),
  //Step
  Color(),
  //Brick
  Color(),
  //TNT
  Color(),
  //Bookcase
  Color(),
  //MossyCobblestone
  Color(),
  //Obsidian
  Color(),
  //Torch
  Color(245,220,50,200),
  //Color(),
  //Fire
  Color(),
  //MobSpawner
  Color(),
  //WoodenStairs
  Color(),
  //Chest
  Color(),
  //RedstoneWire
  Color(),
  //DiamondOre
  Color(),
  //DiamondBlock
  Color(),
  //Workbench
  Color(),
  //Crops
  Color(),
  //Soil
  Color(),
  //Furnace
  Color(),
  //BurningFurnace
  Color(),
  //WoodenDoor
  Color(),
  //Ladder
  Color(),
  //MinecartTracks
  Color(),
  //CobblestoneStairs
  Color(),
  //WallSign
  Color(),
  //Lever
  Color(),
  //StonePressurePlate
  Color(),
  //IronDoor
  Color(),
  //WoodenPressurePlate
  Color(),
  //RedstoneOre
  Color(),
  //GlowingRedstoneOre
  Color(),
  //RedstoneTorchOff
  Color(181,140,64,32),
  //RedstoneTorchOn
  Color(255,0,0,200),
  //StoneButton
  Color(),
  //Snow
  Color(),
  //Ice
  Color(),
  //SnowBlock
  Color(),
  //Cactus
  Color(),
  //Clay
  Color(),
  //Reed
  Color(193,234,150,255),
  //Jukebox
  Color(),
  //Fence,
  Color(),
};

class Level
{
  public:
    // these must be public for the parser to be able to reach them.
    nbt::ByteArray blocks;
    nbt::ByteArray skylight;
    nbt::ByteArray heightmap;
    nbt::ByteArray blocklight;
    nbt::Int xPos;
    nbt::Int zPos;
    bool islevel;

    Level(const char *path);
    ~Level();
    
    Image get_image();
};

#endif /* _LEVEL_H_ */

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include "image/color.hpp"

namespace mc {
  enum MaterialMode {
    Block,
    HalfBlock,
    TorchBlock
  };
  
  enum {
    Air = 0x00,
    Stone = 0x01,
    Grass = 0x02,
    Dirt = 0x03,
    Cobblestone = 0x04,
    Wood = 0x05,
    Sapling = 0x06,
    Bedrock = 0x07,
    Water = 0x08,
    StationaryWater = 0x09,
    Lava = 0x0A,
    StationaryLava = 0x0B,
    Sand = 0x0C,
    Gravel = 0x0D,
    GoldOre = 0x0E,
    IronOre = 0x0F,
    CoalOre = 0x10,
    Log = 0x11,
    Leaves = 0x12,
    Sponge = 0x13,
    Glass = 0x14,
    LapisLazuliOre = 0x15,
    LapisLazuliBlock = 0x16,
    Dispenser = 0x17,
    Sandstone = 0x18,
    NoteBlock = 0x19,
    Bed = 0x1A,
    PoweredRail = 0x1B,
    DetectorRail = 0x1C,
    StickyPistonBase = 0x1D,
    TallGrass = 0x1F,
    DeadShrub = 0x20,
    PistonBase = 0x21,
    PistonPlatform = 0x22,
    Wool = 0x23,
    YellowFlower = 0x25,
    RedRose = 0x26,
    BrownMushroom = 0x28,
    RedMushroom = 0x27,
    GoldBlock = 0x29,
    IronBlock = 0x2A,
    DoubleStep = 0x2B,
    Step = 0x2C,
    Brick = 0x2D,
    TNT = 0x2E,
    Bookcase = 0x2F,
    MossyCobblestone = 0x30,
    Obsidian = 0x31,
    Torch = 0x32,
    Fire = 0x33,
    MobSpawner = 0x34,
    WoodenStairs = 0x35,
    Chest = 0x36,
    RedstoneWire = 0x37,
    DiamondOre = 0x38,
    DiamondBlock = 0x39,
    Workbench = 0x3A,
    Crops = 0x3B,
    Soil = 0x3C,
    Furnace = 0x3D,
    BurningFurnace = 0x3E,
    SignPost = 0x3F,
    WoodenDoor = 0x40,
    Ladder = 0x41,
    MinecartTracks = 0x42,
    CobblestoneStairs = 0x43,
    WallSign = 0x44,
    Lever = 0x45,
    StonePressurePlate = 0x46,
    IronDoor = 0x47,
    WoodenPressurePlate = 0x48,
    RedstoneOre = 0x49,
    GlowingRedstoneOre = 0x4A,
    RedstoneTorchOff = 0x4B,
    RedstoneTorchOn = 0x4C,
    StoneButton = 0x4D,
    Snow = 0x4E,
    Ice = 0x4F,
    SnowBlock = 0x50,
    Cactus = 0x51,
    Clay = 0x52,
    Reed = 0x53,
    Jukebox = 0x54,
    Fence = 0x55,
    Pumpkin = 0x56,
    Bloodstone = 0x57,
    Slowsand = 0x58,
    Lightstone = 0x59,
    Portal = 0x5A,
    Jackolantern = 0x5B,
    Cake = 0x5C,
    Trapdoor = 0x60,
    PineLeaves = 0xEC,
    BirchLeaves = 0xED,
    MaterialCount = 0xEE
  };

  enum {
    WoolWhite = 0x00,
    WoolOrange,
    WoolMagenta,
    WoolLightBlue,
    WoolYellow,
    WoolLightGreen,
    WoolPink,
    WoolGray,
    WoolLightGray,
    WoolCyan,
    WoolPurple,
    WoolBlue,
    WoolBrown,
    WoolDarkGreen,
    WoolRed,
    WoolBlack,
    WoolColorCount
  };
  
  void initialize_constants();
  void deinitialize_constants();
  
  extern const int MapY;
  extern const int MapX;
  extern const int MapZ;
  extern const char **MaterialName;
  extern color* MaterialColor;
  extern color* MaterialSideColor;
  extern color* WoolColor;
  extern enum MaterialMode *MaterialModes;
}

#endif /* _BLOCKS_H_ */

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include "image/color.hpp"

namespace mc {
  enum MaterialMode {
    Block,
    HalfBlock,
    TorchBlock,
    LargeFlowerBlock
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
    Cobweb = 0x1E,
    TallGrass = 0x1F,
    DeadShrub = 0x20,
    PistonBase = 0x21,
    PistonPlatform = 0x22,
    Wool = 0x23,
    PistonMovedBlock = 0x24,
    Dandelion = 0x25,
    SmallFlowers = 0x26,
    BrownMushroom = 0x27,
    RedMushroom = 0x28,
    GoldBlock = 0x29,
    IronBlock = 0x2A,
    DoubleStoneSlab = 0x2B,
    StoneSlab = 0x2C,
    Brick = 0x2D,
    TNT = 0x2E,
    Bookcase = 0x2F,
    MossyCobblestone = 0x30,
    Obsidian = 0x31,
    Torch = 0x32,
    Fire = 0x33,
    MobSpawner = 0x34,
    OakWoodStairs = 0x35,
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
    RedstoneRepeaterOff = 0x5D,
    RedstoneRepeaterOn = 0x5E,
    StainedGlass = 0x5F,
    Trapdoor = 0x60,
    EggBlock = 0x61,
    StoneBrick = 0x62,
    HugeBrownMushroom = 0x63,
    HugeRedMushroom = 0x64,
    IronBars = 0x65,
    GlassPane = 0x66,
    Melon = 0x67,
    PumpkinStem = 0x68,
    MelonStem = 0x69,
    Vines = 0x6A,
    FenceGate = 0x6B,
    BrickStairs = 0x6C,
    StoneBrickStairs = 0x6D,
    Mycelium = 0x6E,
    LilyPad = 0x6F,
    NetherBrick = 0x70,
    NetherBrickFence = 0x71,
    NetherBrickStairs = 0x72,
    NetherWart = 0x73,
    EnchantmentTable = 0x74,
    BrewingStand = 0x75,
    Cauldron = 0x76,
    EndPortal = 0x77,
    EndPortalFrame = 0x78,
    EndStone = 0x79,
    DragonEgg = 0x7A,
    RedstoneLampOff = 0x7B,
    RedstoneLampOn = 0x7C,
    WoodenDoubleSlab = 0x7D,
    WoodenSlab = 0x7E,
    Cocoa = 0x7F,
    SandstoneStairs = 0x80,
    EmeraldOre = 0x81,
    EnderChest = 0x82,
    TripwireHook = 0x83,
    Tripwire = 0x84,
    EmeraldBlock = 0x85,
    SpruceWoodStairs = 0x86,
    BirchWoodStairs = 0x87,
    JungleWoodStairs = 0x88,
    CommandBlock = 0x89,
    Beacon = 0x8A,
    CobblestoneWall = 0x8B,
    FlowerPot = 0x8C,
    Carrots = 0x8D,
    Potatoes = 0x8E,
    WoodenButton = 0x8F,
    MobHead = 0x90,
    Anvil = 0x91,
    TrappedChest = 0x92,
    WeightedPressurePlateLight = 0x93,
    WeightedPressurePlateHeavy = 0x94,
    RedstoneComparatorOff = 0x95,
    RedstoneComparatorOn = 0x96,
    DaylightSensor = 0x97,
    RedstoneBlock = 0x98,
    NetherQuartzOre = 0x99,
    Hopper = 0x9A,
    QuartzBlock = 0x9B,
    QuartzStairs = 0x9C,
    ActivatorRail = 0x9D,
    Dropper = 0x9E,
    StainedClay = 0x9F,
    StainedGlassPane = 0xA0,
    Leaves2 = 0xA1,
    Log2 = 0xA2,
    AcaciaWoodStairs = 0xA3,
    DarkOakWoodStairs = 0xA4,
    //SlimeBlock = 0xA5,
    //Barrier = 0xA6,
    //IronTrapdoor = 0xA7,
    //UNUSED = 0xA8,
    //UNUSED = 0xA9,
    HayBlock = 0xAA,
    Carpet = 0xAB,
    HardenedClay = 0xAC,
    CoalBlock = 0xAD,
    PackedIce = 0xAE,
    LargeFlowers = 0xAF,
    PineLeaves = 0xEC,
    BirchLeaves = 0xED,
    MaterialCount = 0x1000
  };

  enum {
    DirtNormal = 0x00,
    DirtCoarse,
    DirtPodzol,
    DirtColorCount
  };

  enum {
    WoodOak = 0x00,
    WoodSpruce,
    WoodBirch,
    WoodJungle,
    WoodAcacia,
    WoodDarkOak,
    WoodColorCount
  };

  enum {
    SandNormal = 0x00,
    SandRed,
    SandColorCount
  };

  enum {
    LogUpDown = 0x00 - WoodOak,
    LogEastWest = 0x04 - WoodOak,
    LogNorthSouth = 0x08 - WoodOak,
    LogOnlyBark = 0x0C - WoodOak
  };

  enum {
    LeavesNormal = 0x00,
    LeavesNoDecay = 0x04,
    LeavesCheckDecay = 0x08,
    LeavesNoDecayCheckDecay = 0x0C
  };

  enum {
    ColorWhite = 0x00,
    ColorOrange,
    ColorMagenta,
    ColorLightBlue,
    ColorYellow,
    ColorLightGreen,
    ColorPink,
    ColorGray,
    ColorLightGray,
    ColorCyan,
    ColorPurple,
    ColorBlue,
    ColorBrown,
    ColorDarkGreen,
    ColorRed,
    ColorBlack,
    ColorColorCount
  };

  enum {
    SFlowersPoppy = 0x00,
    SFlowersBlueOrchid,
    SFlowersAllium,
    SFlowersAzureBluet,
    SFlowersRedTulip,
    SFlowersOrangeTulip,
    SFlowersWhiteTulip,
    SFlowersPinkTulip,
    SFlowersOxeyeDaisy,
    SFlowersColorCount
  };

  enum {
    SlabStone = 0x00,
    SlabSandstone,
    SlabWood,
    SlabCobblestone,
    SlabBrick,
    SlabStoneBrick,
    SlabNether,
    SlabQuartz,
    SlabReverse,
    SlabFullStone = SlabReverse,
    SlabFullSandStone
  };

  enum {
    WoodenSlabNormal = 0x00,
    WoodenSlabReverse = 0x08
  };

  enum {
    Leaves2Normal = 0x00 - WoodAcacia,
    Leaves2NoDecay = 0x04 - WoodAcacia,
    Leaves2CheckDecay = 0x08 - WoodAcacia,
    Leaves2NoDecayCheckDecay = 0x0C - WoodAcacia
  };

  enum {
    Log2UpDown = 0x00 - WoodAcacia,
    Log2EastWest = 0x04 - WoodAcacia,
    Log2NorthSouth = 0x08 - WoodAcacia,
    Log2OnlyBark = 0x0C - WoodAcacia
  };

  enum {
    LFlowersSunflower = 0x00,
    LFlowersLilac,
    LFlowersDoubleTallgrass,
    LFlowersLargeFern,
    LFlowersRoseBush,
    LFlowersPeony,
    LFlowersUnused1,
    LFlowersUnsued2,
    LFlowersTopHalf = 0x08
  };

  void initialize_constants();
  void deinitialize_constants();

  extern const color SharedInvisColor;
  extern const color SharedDefaultColor;

  typedef struct {
    int count;
    color* top;
    color* side;
  } MaterialColorT;

  extern const int MapY;
  extern const int MapX;
  extern const int MapZ;
  extern const char **MaterialName;
  extern MaterialColorT *MaterialColorData;
  extern enum MaterialMode *MaterialModes;

  inline color get_color(int material) {
    return MaterialColorData[material].top[0];
  }

  inline color get_side_color(int material) {
    return MaterialColorData[material].side[0];
  }

  inline color get_color(int material, int data) {
    if (data >= MaterialColorData[material].count) {
      data = 0;
    }
    return MaterialColorData[material].top[data];
  }

  inline color get_side_color(int material, int data) {
    if (data >= MaterialColorData[material].count) {
      data = 0;
    }
    return MaterialColorData[material].side[data];
  }
  
  /**
   * Sets the color values for a material at the specified index.
   * Notice: the side color is drakened with value 50 automatically, unless
   * you specifiy the darken parameter as false.
   * Hint: use SharedInvisColor for the side value to copy the top color
   */
  void set_color(int material, int idx, color top,
          color side=SharedInvisColor, bool darken=true);
}

#endif /* _BLOCKS_H_ */

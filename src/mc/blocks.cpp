// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "blocks.hpp"

namespace mc {
  const color SharedInvisColor = color(0, 0, 0, 0);
  const color SharedDefaultColor = color(0, 0, 0, 0xff);

  const int MapX = 0x10;
  const int MapZ = 0x10;
  const int MapY = 0x100;

  const char *DefaultName = "None";
  
  const char **MaterialName;
  MaterialColorT *MaterialColorData;
  MaterialMode *MaterialModes;
  
  void set_color(int material, int data, color top, color side, bool darken)
  {
    MaterialColorData[material].count =
      data >= MaterialColorData[material].count ?
      1+data : MaterialColorData[material].count;

    MaterialColorData[material].top[data] = top;

    if (side == SharedInvisColor) {
      MaterialColorData[material].side[data] = color(top);
    } else {
      MaterialColorData[material].side[data] = color(side);
    }

    if (darken) {
      MaterialColorData[material].side[data].darken(0x20);
    }
  }

  /*color get_color(int bt, int dv){
    switch (bt){
	case Wool:
		return WoolColor[dv];
	break;
	case Step:
		return StepColor[dv];
	break;
	default:
		return MaterialColor[bt];
    } 
  }*/

  void initialize_constants() {
    MaterialName = new const char*[MaterialCount];
    MaterialColorData = new MaterialColorT[MaterialCount];
    MaterialModes = new enum MaterialMode[MaterialCount];
    for (int i = 0; i < MaterialCount; i++) {
      MaterialName[i] = DefaultName;
      MaterialColorData[i].top = new color[16];
      MaterialColorData[i].side = new color[16];
      MaterialColorData[i].count = 0;
      set_color(i, 0, SharedDefaultColor);
      MaterialModes[i] = Block;
    }
    MaterialName[Air] = "Air";
    MaterialName[Stone] = "Stone";
    MaterialName[Grass] = "Grass";
    MaterialName[Dirt] = "Dirt";
    MaterialName[Cobblestone] = "Cobblestone";
    MaterialName[Wood] = "Wood";
    MaterialName[Sapling] = "Sapling";
    MaterialName[Bedrock] = "Bedrock";
    MaterialName[Water] = "Water";
    MaterialName[StationaryWater] = "StationaryWater";
    MaterialName[Lava] = "Lava";
    MaterialName[StationaryLava] = "StationaryLava";
    MaterialName[Sand] = "Sand";
    MaterialName[Gravel] = "Gravel";
    MaterialName[GoldOre] = "GoldOre";
    MaterialName[IronOre] = "IronOre";
    MaterialName[CoalOre] = "CoalOre";
    MaterialName[Log] = "Log";
    MaterialName[Leaves] = "Leaves";
    MaterialName[Sponge] = "Sponge";
    MaterialName[Glass] = "Glass";
    MaterialName[LapisLazuliOre] = "LapisLazuliOre";
    MaterialName[LapisLazuliBlock] = "LapisLazuliBlock";
    MaterialName[Dispenser] = "Dispenser";
    MaterialName[Sandstone] = "Sandstone";
    MaterialName[NoteBlock] = "NoteBlock";
    MaterialName[Bed] = "Bed";
    MaterialName[PoweredRail] = "PoweredRail";
    MaterialName[DetectorRail] = "DetectorRail";
    MaterialName[StickyPistonBase] = "StickyPistonBase";
    MaterialName[TallGrass] = "TallGrass";
    MaterialName[DeadShrub] = "DeadShrub";
    MaterialName[PistonBase] = "PistonBase";
    MaterialName[PistonPlatform] = "PistonPlatform";
    MaterialName[Wool] = "Wool";
    MaterialName[YellowFlower] = "YellowFlower";
    MaterialName[RedRose] = "RedRose";
    MaterialName[BrownMushroom] = "BrownMushroom";
    MaterialName[RedMushroom] = "RedMushroom";
    MaterialName[GoldBlock] = "GoldBlock";
    MaterialName[IronBlock] = "IronBlock";
    MaterialName[DoubleStep] = "DoubleStep";
    MaterialName[Step] = "Step";
    MaterialName[Brick] = "Brick";
    MaterialName[TNT] = "TNT";
    MaterialName[Bookcase] = "Bookcase";
    MaterialName[MossyCobblestone] = "MossyCobblestone";
    MaterialName[Obsidian] = "Obsidian";
    MaterialName[Torch] = "Torch";
    MaterialName[Fire] = "Fire";
    MaterialName[MobSpawner] = "MobSpawner";
    MaterialName[WoodenStairs] = "WoodenStairs";
    MaterialName[Chest] = "Chest";
    MaterialName[RedstoneWire] = "RedstoneWire";
    MaterialName[DiamondOre] = "DiamondOre";
    MaterialName[DiamondBlock] = "DiamondBlock";
    MaterialName[Workbench] = "Workbench";
    MaterialName[Crops] = "Crops";
    MaterialName[Soil] = "Soil";
    MaterialName[Furnace] = "Furnace";
    MaterialName[BurningFurnace] = "BurningFurnace";
    MaterialName[SignPost] = "SignPost";
    MaterialName[WoodenDoor] = "WoodenDoor";
    MaterialName[Ladder] = "Ladder";
    MaterialName[MinecartTracks] = "MinecartTracks";
    MaterialName[CobblestoneStairs] = "CobblestoneStairs";
    MaterialName[WallSign] = "WallSign";
    MaterialName[Lever] = "Lever";
    MaterialName[StonePressurePlate] = "StonePressurePlate";
    MaterialName[IronDoor] = "IronDoor";
    MaterialName[WoodenPressurePlate] = "WoodenPressurePlate";
    MaterialName[RedstoneOre] = "RedstoneOre";
    MaterialName[GlowingRedstoneOre] = "GlowingRedstoneOre";
    MaterialName[RedstoneTorchOff] = "RedstoneTorchOff";
    MaterialName[RedstoneTorchOn] = "RedstoneTorchOn";
    MaterialName[StoneButton] = "StoneButton";
    MaterialName[Snow] = "Snow";
    MaterialName[Ice] = "Ice";
    MaterialName[SnowBlock] = "SnowBlock";
    MaterialName[Cactus] = "Cactus";
    MaterialName[Clay] = "Clay";
    MaterialName[Reed] = "Reed";
    MaterialName[Jukebox] = "Jukebox";
    MaterialName[Fence] = "Fence";
    MaterialName[Pumpkin] = "Pumpkin";
    MaterialName[Bloodstone] = "Bloodstone";
    MaterialName[Slowsand] = "Slowsand";
    MaterialName[Lightstone] = "Lightstone";
    MaterialName[Trapdoor] = "Trapdoor";
    MaterialName[Portal] = "Portal";
    MaterialName[Jackolantern] = "Jackolantern";
    MaterialName[RedstoneRepeaterOn] = "RedstoneRepeaterOn";
    MaterialName[RedstoneRepeaterOff] = "RedstoneRepeaterOff";
    MaterialName[Cake] = "Cake";
    MaterialName[EggBlock] = "EggBlock";
    MaterialName[StoneBrick] = "StoneBrick";
    MaterialName[HugeRedMushroom] = "HugeRedMushroom";
    MaterialName[HugeBrownMushroom] = "HugeBrownMushroom";
    MaterialName[IronBars] = "IronBars";
    MaterialName[GlassPane] = "GlassPane";
    MaterialName[Melon] = "Melon";
    MaterialName[PumpkinStem] = "PumpkinStem";
    MaterialName[MelonStem] = "MelonStem";
    MaterialName[LilyPad] = "LilyPad";
    MaterialName[NetherBrick] = "NetherBrick";
    MaterialName[NetherBrickFence] = "NetherBrickFence";
    MaterialName[NetherBrickStairs] = "NetherBrickStairs";
    MaterialName[NetherWart] = "NetherWart";
    MaterialName[EnchantmentTable] = "EnchantmentTable";
    MaterialName[BrewingStand] = "BrewingStand";
    MaterialName[Cauldron] = "Cauldron";
    MaterialName[EndPortal] = "EndPortal";
    MaterialName[EndPortalFrame] = "EndPortalFrame";
    MaterialName[EndStone] = "EndStone";
    MaterialName[DragonEgg] = "DragonEgg";
    MaterialName[Mycelium] = "Mycelium";
    MaterialName[Vines] = "Vines";
    MaterialName[FenceGate] = "FenceGate";
    MaterialName[BrickStairs] = "BrickStairs";
    MaterialName[StoneBrickStairs] = "StoneBrickStairs";
    MaterialName[PineLeaves] = "PineLeaves";
    MaterialName[BirchLeaves] = "BirchLeaves";
    
    set_color(Air, 0, color(255,255,255,0), SharedInvisColor, false);
    set_color(Stone, 0, color(128,128,128,255));
    set_color(Dirt, 0, color(134,96,67,255));
    set_color(Grass, 0, color(120,172,70,255), get_color(Dirt));
    set_color(Cobblestone, 0, color(100,100,100,255));
    set_color(Wood, 0, color(157,128,79,255));
    set_color(Sapling, 0, color(120,120,120,0));
    set_color(Bedrock, 0, color(84,84,84,255));
    set_color(Water, 0, color(56,68,127,64), SharedInvisColor, false);
    set_color(StationaryWater, 0, color(56,68,127,64), SharedInvisColor, false);
    set_color(Lava, 0, color(255,90,0,255));
    set_color(StationaryLava, 0, color(255,90,0,255));
    set_color(Sand, 0, color(218,210,158,255));
    set_color(Gravel, 0, color(136,126,126,255));
    set_color(GoldOre, 0, color(143,140,125,255));
    set_color(IronOre, 0, color(136,130,127,255));
    set_color(CoalOre, 0, color(115,115,115,255));
    set_color(Log, 0, color(102,81,51,255));
    set_color(Leaves, 0, color(0x4a,0x83,0x42,0x80));
    set_color(Sponge, 0, color(0xc3,0xc3,0x32,0xff));
    set_color(Glass, 0, color(255,255,255,48));
    set_color(LapisLazuliOre, 0, color(102,112,134,255));
    set_color(LapisLazuliBlock, 0, color(29,71,165,255));
    set_color(Dispenser, 0, color(107,107,107,255));
    set_color(Sandstone, 0, get_color(Sand));
    set_color(NoteBlock, 0, color(100,67,50,255));
    set_color(Bed, 0, color(150,0,0,255));
    set_color(PoweredRail, 0, color(120, 120, 120, 128), color(255,220,0,128));
    set_color(DetectorRail, 0, get_color(PoweredRail), color(230,0,0,128));
    set_color(StickyPistonBase, 0, color(157,192,79,255));
    set_color(TallGrass, 0,
      color(0x90, 0xbc, 0x27, 0xff), color(0x90, 0xbc, 0x27, 0xff));
    set_color(DeadShrub, 0, get_color(Wood));
    set_color(PistonBase, 0, get_color(Wood));
    set_color(PistonPlatform, 0, get_color(Air));
    set_color(YellowFlower, 0, color(255,255,0,255));
    set_color(RedRose, 0, color(255,0,0,255));
    set_color(BrownMushroom, 0, color(200, 200, 0, 255));
    set_color(RedMushroom, 0, color(255, 0, 0, 255));
    set_color(GoldBlock, 0, color(0xff, 0xed, 0x8c, 0xff));
    set_color(IronBlock, 0, color(0xd9, 0xd9, 0xd9, 0xff));
    set_color(DoubleStep, 0, color(200,200,200,255));
    set_color(Step, 0, color(200,200,200,255));
    set_color(Brick, 0, color(0x56, 0x23, 0x17, 0xff));
    set_color(TNT, 0, color(0xff, 0x0, 0x0, 0xff));
    set_color(Bookcase, 0, color(0xbf, 0xa9, 0x74, 0xff));
    set_color(MossyCobblestone, 0, color(0x7f, 0xae, 0x7d, 0xff));
    set_color(Obsidian, 0, color(0x11, 0x0d, 0x1a, 0xff));
    set_color(Torch, 0, color(0xff, 0xe1, 0x60,0xd0), SharedInvisColor, false);
    set_color(Fire, 0, color(0xe0, 0xae, 0x15, 0xff));
    set_color(MobSpawner, 0, color(0xff, 0xff, 0xff, 0x00));
    set_color(WoodenStairs, 0, color(0xbf, 0xa9, 0x74, 0xff));
    set_color(Chest, 0, color(0xbf, 0x87, 0x02, 0xff));
    set_color(RedstoneWire, 0, color(0x6f, 0x01, 0x01, 0xff));
    set_color(DiamondOre, 0, color(129,140,143,255));
    set_color(DiamondBlock, 0, color(45,166,152,255));
    set_color(Workbench, 0, color(0xa9, 0x6b, 0x00, 0xff));
    set_color(Crops, 0, color(0x90, 0xbc, 0x27, 0xff));
    set_color(Soil, 0, get_color(Dirt));
    set_color(Furnace, 0, color(0xbc, 0xbc, 0xbc, 0xff));
    set_color(BurningFurnace, 0, color(0xdd, 0xdd, 0xdd, 0xff));
    set_color(SignPost, 0, SharedInvisColor);
    set_color(WoodenDoor, 0, SharedInvisColor);
    set_color(Ladder, 0, color(0xff, 0xc8, 0x8c, 0));
    set_color(MinecartTracks, 0, get_color(PoweredRail));
    set_color(CobblestoneStairs, 0, color(120, 120, 120, 128));
    set_color(WallSign, 0, SharedInvisColor);
    set_color(Lever, 0, SharedInvisColor);
    set_color(StonePressurePlate, 0, color(120,120,120,255));
    set_color(IronDoor, 0, SharedInvisColor);
    set_color(WoodenPressurePlate, 0, SharedInvisColor);
    set_color(RedstoneOre, 0, color(143,125,125,0xff));
    set_color(GlowingRedstoneOre, 0, color(163,145,145,0xff));
    set_color(RedstoneTorchOff, 0, color(181,140,64,32),SharedInvisColor,false);
    set_color(RedstoneTorchOn, 0, color(255,0,0,0xb0), SharedInvisColor, false);
    set_color(StoneButton, 0, SharedInvisColor);
    set_color(Snow, 0, color(255, 255, 255, 255));
    set_color(Ice, 0, color(120, 120, 255, 120));
    set_color(SnowBlock, 0, color(255, 255, 255, 255));
    set_color(Cactus, 0, color(85,107,47,255));
    set_color(Clay, 0, color(0x90, 0x98, 0xa8, 0xff));
    set_color(Reed, 0, color(193,234,150,255));
    set_color(Jukebox, 0, color(0x7d, 0x42, 0x2c, 0xff));
    set_color(Fence, 0, color(0x58, 0x36, 0x16, 200));
    set_color(Pumpkin, 0, color(0xe3, 0x90, 0x1d, 0xff));
    set_color(Bloodstone, 0, color(0xc2, 0x73, 0x73, 0xff));
    set_color(Slowsand, 0, color(0x79, 0x61, 0x52, 0xff));
    set_color(Lightstone, 0, color(0xff, 0xbc, 0x5e, 0xff));
    set_color(Trapdoor, 0, get_color(WoodenPressurePlate));
    set_color(Portal, 0, color(0x3c, 0x0d, 0x6a, 0x7f));
    set_color(Jackolantern, 0, get_color(Pumpkin));
    set_color(RedstoneRepeaterOn, 0, get_color(RedstoneWire));
    set_color(RedstoneRepeaterOff, 0, get_color(RedstoneWire));
    set_color(Cake, 0, color(228,205,206,255));
    set_color(EggBlock, 0, get_color(Stone));
    set_color(StoneBrick, 0, get_color(Stone));
    set_color(HugeRedMushroom, 0, color(183,31,29,0xff));
    set_color(HugeBrownMushroom, 0, color(206,174,123,0xff));
    set_color(IronBars, 0, get_color(IronBlock));
    set_color(GlassPane, 0, get_color(Glass));
    set_color(Melon, 0, color(50,200,45,192));
    set_color(PumpkinStem, 0, color(0x00, 0x00, 0x00, 0x00));
    set_color(MelonStem, 0, color(0x00, 0x00, 0x00, 0x00));
    set_color(LilyPad, 0, color(50,89,45,128), color(50,89,45,128));
    set_color(NetherBrick, 0, color(66,32,38,255));
    set_color(NetherBrickFence, 0, color(66,32,38,200));
    set_color(NetherBrickStairs, 0, get_color(NetherBrick));
    set_color(NetherWart, 0, color(149,21,8,255));
    set_color(EnchantmentTable, 0, color(130,5,5,255));
    set_color(BrewingStand, 0, color(124,118,51,255));
    set_color(Cauldron, 0, color(49,49,49,255));
    set_color(EndPortal, 0, color(35,60,99,128));
    set_color(EndPortalFrame, 0, color(62,115,105,255));
    set_color(EndStone, 0, color(203,206,148,255));
    set_color(DragonEgg, 0, color(72,4,82,255));
    set_color(Mycelium, 0, color(110,93,133,255));
    set_color(Vines, 0, color(50,89,45,128), color(50,89,45,128));
    set_color(FenceGate, 0, get_color(Fence));
    set_color(BrickStairs, 0, get_color(Brick));
    set_color(StoneBrickStairs, 0, get_color(Stone));
    set_color(PineLeaves, 0, color(50,89,45,128));
    set_color(BirchLeaves, 0, color(94,167,84,128));

    MaterialModes[Air] = Block;
    MaterialModes[Stone] = Block;
    MaterialModes[Grass] = Block;
    MaterialModes[Dirt] = Block;
    MaterialModes[Cobblestone] = Block;
    MaterialModes[Wood] = Block;
    MaterialModes[Sapling] = Block;
    MaterialModes[Bedrock] = Block;
    MaterialModes[Water] = Block;
    MaterialModes[StationaryWater] = Block;
    MaterialModes[Lava] = Block;
    MaterialModes[StationaryLava] = Block;
    MaterialModes[Sand] = Block;
    MaterialModes[Gravel] = Block;
    MaterialModes[GoldOre] = Block;
    MaterialModes[IronOre] = Block;
    MaterialModes[CoalOre] = Block;
    MaterialModes[Log] = Block;
    MaterialModes[Leaves] = Block;
    MaterialModes[Sponge] = Block;
    MaterialModes[Glass] = Block;
    MaterialModes[LapisLazuliOre] = Block;
    MaterialModes[LapisLazuliBlock] = Block;
    MaterialModes[Dispenser] = Block;
    MaterialModes[Sandstone] = Block;
    MaterialModes[NoteBlock] = Block;
    MaterialModes[Bed] = Block;
    MaterialModes[PoweredRail] = Block;
    MaterialModes[DetectorRail] = Block;
    MaterialModes[StickyPistonBase] = Block;
    MaterialModes[TallGrass] = Block;
    MaterialModes[DeadShrub] = TorchBlock;
    MaterialModes[PistonBase] = Block;
    MaterialModes[PistonPlatform] = Block;
    MaterialModes[Wool] = Block;
    MaterialModes[YellowFlower] = Block;
    MaterialModes[RedRose] = Block;
    MaterialModes[BrownMushroom] = Block;
    MaterialModes[RedMushroom] = Block;
    MaterialModes[GoldBlock] = Block;
    MaterialModes[IronBlock] = Block;
    MaterialModes[DoubleStep] = Block;
    MaterialModes[Step] = HalfBlock;
    MaterialModes[Brick] = Block;
    MaterialModes[TNT] = Block;
    MaterialModes[Bookcase] = Block;
    MaterialModes[MossyCobblestone] = Block;
    MaterialModes[Obsidian] = Block;
    MaterialModes[Torch] = TorchBlock;
    MaterialModes[Fire] = Block;
    MaterialModes[MobSpawner] = Block;
    MaterialModes[WoodenStairs] = Block;
    MaterialModes[Chest] = Block;
    MaterialModes[RedstoneWire] = Block;
    MaterialModes[DiamondOre] = Block;
    MaterialModes[DiamondBlock] = Block;
    MaterialModes[Workbench] = Block;
    MaterialModes[Crops] = Block;
    MaterialModes[Soil] = Block;
    MaterialModes[Furnace] = Block;
    MaterialModes[BurningFurnace] = Block;
    MaterialModes[SignPost] = Block;
    MaterialModes[WoodenDoor] = Block;
    MaterialModes[Ladder] = Block;
    MaterialModes[MinecartTracks] = Block;
    MaterialModes[CobblestoneStairs] = Block;
    MaterialModes[WallSign] = Block;
    MaterialModes[Lever] = Block;
    MaterialModes[StonePressurePlate] = HalfBlock;
    MaterialModes[IronDoor] = Block;
    MaterialModes[WoodenPressurePlate] = HalfBlock;
    MaterialModes[RedstoneOre] = Block;
    MaterialModes[GlowingRedstoneOre] = Block;
    MaterialModes[RedstoneTorchOff] = TorchBlock;
    MaterialModes[RedstoneTorchOn] = TorchBlock;
    MaterialModes[StoneButton] = Block;
    MaterialModes[Snow] = HalfBlock;
    MaterialModes[Ice] = Block;
    MaterialModes[SnowBlock] = Block;
    MaterialModes[Cactus] = Block;
    MaterialModes[Clay] = Block;
    MaterialModes[Reed] = Block;
    MaterialModes[Jukebox] = Block;
    MaterialModes[Fence] = Block;
    MaterialModes[Pumpkin] = Block;
    MaterialModes[Bloodstone] = Block;
    MaterialModes[Slowsand] = Block;
    MaterialModes[Lightstone] = Block;
    MaterialModes[Trapdoor] = HalfBlock;
    MaterialModes[Portal] = Block;
    MaterialModes[Jackolantern] = Block;
    MaterialModes[Cake] = HalfBlock;
    MaterialModes[RedstoneRepeaterOn] = Block;
    MaterialModes[RedstoneRepeaterOff] = Block;
    MaterialModes[EggBlock] = Block;
    MaterialModes[StoneBrick] = Block;
    MaterialModes[HugeRedMushroom] = Block;
    MaterialModes[HugeBrownMushroom] = Block;
    MaterialModes[IronBars] = Block;
    MaterialModes[GlassPane] = Block;
    MaterialModes[Melon] = Block;
    MaterialModes[PumpkinStem] = Block;
    MaterialModes[MelonStem] = Block;
    MaterialModes[Vines] = Block;
    MaterialModes[LilyPad] = Block; 
    MaterialModes[Mycelium] = Block;
    MaterialModes[NetherBrick] = Block;
    MaterialModes[NetherBrickFence] = Block;
    MaterialModes[NetherBrickStairs] = Block;
    MaterialModes[NetherWart] = Block;
    MaterialModes[EnchantmentTable] = Block;
    MaterialModes[BrewingStand] = Block;
    MaterialModes[Cauldron] = Block;
    MaterialModes[EndPortal] = Block;
    MaterialModes[EndPortalFrame] = Block;
    MaterialModes[EndStone] = Block;
    MaterialModes[DragonEgg] = Block;
    MaterialModes[FenceGate] = Block;
    MaterialModes[BrickStairs] = Block;
    MaterialModes[StoneBrickStairs] = Block;
    MaterialModes[PineLeaves] = Block;
    MaterialModes[BirchLeaves] = Block;

    /* 
     * Special colors depending on data value
     * Start with the highest index to reduce allocation time complexity 
     * The order of the following entries does not matter.
     */
    set_color(Wool, WoolBlack, color(27, 23, 23, 255));
    set_color(Wool, WoolWhite, color(223, 223, 223, 255));
    set_color(Wool, WoolOrange, color(234, 128, 55, 255));
    set_color(Wool, WoolMagenta, color(191, 76, 201, 255));
    set_color(Wool, WoolLightBlue, color(105, 139, 212, 255));
    set_color(Wool, WoolYellow, color(195, 181, 28, 255));
    set_color(Wool, WoolLightGreen, color(59, 189, 48, 255));
    set_color(Wool, WoolPink, color(218, 132, 155, 255));
    set_color(Wool, WoolGray, color(67, 67, 67, 255));
    set_color(Wool, WoolLightGray, color(159, 166, 166, 255));
    set_color(Wool, WoolCyan, color(39, 117, 150, 255));
    set_color(Wool, WoolPurple, color(130, 54, 196, 255));
    set_color(Wool, WoolBlue, color(39, 51, 154, 255));
    set_color(Wool, WoolBrown, color(86, 51, 28, 255));
    set_color(Wool, WoolDarkGreen, color(56, 77, 24, 255));
    set_color(Wool, WoolRed, color(164, 45, 41, 255));

    set_color(Step, StepCobblestone, get_color(Cobblestone));
    set_color(Step, StepStone, get_color(Stone));
    set_color(Step, StepSandstone, get_color(Sandstone));
    set_color(Step, StepWood, get_color(Wood));

    set_color(DoubleStep, StepCobblestone, get_color(Cobblestone));
    set_color(DoubleStep, StepStone, get_color(Stone));
    set_color(DoubleStep, StepSandstone, get_color(Sandstone));
    set_color(DoubleStep, StepWood, get_color(Wood));
  }
  
  void deinitialize_constants() {
    delete [] MaterialColorData;
  }
}

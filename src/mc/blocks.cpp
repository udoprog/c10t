// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "blocks.hpp"

namespace mc {
  const color SharedInvisColor = color(0, 0, 0, 0);
  const color SharedDefaultColor = color(0, 0, 0, 0xff);

  const int MapX = 0x10;
  const int MapZ = 0x10;
  const int MapY = 0x80;

  const char *DefaultName = "None";
  
  const char **MaterialName;
  MaterialColorT *MaterialColorData;
  
  MaterialMode *MaterialModes;

  void initialize_constants() {
    MaterialName = new const char*[MaterialCount];
    MaterialColorData = new MaterialColorT[MaterialCount];
    MaterialModes = new enum MaterialMode[MaterialCount];

    for (int i = 0; i < MaterialCount; i++) {
      MaterialName[i] = DefaultName;
      MaterialColorData[i].top = new color[16];
      MaterialColorData[i].side = new color[16];
      MaterialColorData[i].count = 0;
      setColor(i, 0, SharedDefaultColor);
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
    MaterialName[Mycelium] = "Mycelium";
    MaterialName[Vines] = "Vines";
    MaterialName[FenceGate] = "FenceGate";
    MaterialName[BrickStairs] = "BrickStairs";
    MaterialName[StoneBrickStairs] = "StoneBrickStairs";
    MaterialName[PineLeaves] = "PineLeaves";
    MaterialName[BirchLeaves] = "BirchLeaves";
    
    setColor(Air, 0, color(255,255,255,0), SharedInvisColor, false);
    setColor(Stone, 0, color(128,128,128,255));
    setColor(Dirt, 0, color(134,96,67,255));
    setColor(Grass, 0, color(120,172,70,255), getColor(Dirt));
    setColor(Cobblestone, 0, color(100,100,100,255));
    setColor(Wood, 0, color(157,128,79,255));
    setColor(Sapling, 0, color(120,120,120,0));
    setColor(Bedrock, 0, color(84,84,84,255));
    setColor(Water, 0, color(56,68,127,64), SharedInvisColor, false);
    setColor(StationaryWater, 0, color(56,68,127,64), SharedInvisColor, false);
    setColor(Lava, 0, color(255,90,0,255));
    setColor(StationaryLava, 0, color(255,90,0,255));
    setColor(Sand, 0, color(218,210,158,255));
    setColor(Gravel, 0, color(136,126,126,255));
    setColor(GoldOre, 0, color(143,140,125,255));
    setColor(IronOre, 0, color(136,130,127,255));
    setColor(CoalOre, 0, color(115,115,115,255));
    setColor(Log, 0, color(102,81,51,255));
    setColor(Leaves, 0, color(0x4a,0x83,0x42,0x80));
    setColor(Sponge, 0, color(0xc3,0xc3,0x32,0xff));
    setColor(Glass, 0, color(255,255,255,48));
    setColor(LapisLazuliOre, 0, color(102,112,134,255));
    setColor(LapisLazuliBlock, 0, color(29,71,165,255));
    setColor(Dispenser, 0, color(107,107,107,255));
    setColor(Sandstone, 0, getColor(Sand));
    setColor(NoteBlock, 0, color(100,67,50,255));
    setColor(Bed, 0, color(150,0,0,255));
    setColor(PoweredRail, 0, color(120, 120, 120, 128), color(255,220,0,128));
    setColor(DetectorRail, 0, getColor(PoweredRail), color(230,0,0,128));
    setColor(StickyPistonBase, 0, color(157,192,79,255));
    setColor(TallGrass, 0,
      color(0x90, 0xbc, 0x27, 0xff), color(0x90, 0xbc, 0x27, 0xff));
    setColor(DeadShrub, 0, getColor(Wood));
    setColor(PistonBase, 0, getColor(Wood));
    setColor(PistonPlatform, 0, getColor(Air));
    setColor(YellowFlower, 0, color(255,255,0,255));
    setColor(RedRose, 0, color(255,0,0,255));
    setColor(BrownMushroom, 0, SharedInvisColor);
    setColor(RedMushroom, 0, SharedInvisColor);
    setColor(GoldBlock, 0, color(0xff, 0xed, 0x8c, 0xff));
    setColor(IronBlock, 0, color(0xd9, 0xd9, 0xd9, 0xff));
    setColor(DoubleStep, 0, color(200,200,200,255));
    setColor(Step, 0, color(200,200,200,255));
    setColor(Brick, 0, color(0x56, 0x23, 0x17, 0xff));
    setColor(TNT, 0, color(0xff, 0x0, 0x0, 0xff));
    setColor(Bookcase, 0, color(0xbf, 0xa9, 0x74, 0xff));
    setColor(MossyCobblestone, 0, color(0x7f, 0xae, 0x7d, 0xff));
    setColor(Obsidian, 0, color(0x11, 0x0d, 0x1a, 0xff));
    setColor(Torch, 0, color(0xff, 0xe1, 0x60,0xd0), SharedInvisColor, false);
    setColor(Fire, 0, color(0xe0, 0xae, 0x15, 0xff));
    setColor(MobSpawner, 0, color(0xff, 0xff, 0xff, 0x00));
    setColor(WoodenStairs, 0, color(0xbf, 0xa9, 0x74, 0xff));
    setColor(Chest, 0, color(0xbf, 0x87, 0x02, 0xff));
    setColor(RedstoneWire, 0, color(0x6f, 0x01, 0x01, 0xff));
    setColor(DiamondOre, 0, color(129,140,143,255));
    setColor(DiamondBlock, 0, color(45,166,152,255));
    setColor(Workbench, 0, color(0xa9, 0x6b, 0x00, 0xff));
    setColor(Crops, 0, color(0x90, 0xbc, 0x27, 0xff));
    setColor(Soil, 0, getColor(Dirt));
    setColor(Furnace, 0, color(0xbc, 0xbc, 0xbc, 0xff));
    setColor(BurningFurnace, 0, color(0xdd, 0xdd, 0xdd, 0xff));
    setColor(SignPost, 0, SharedInvisColor);
    setColor(WoodenDoor, 0, SharedInvisColor);
    setColor(Ladder, 0, color(0xff, 0xc8, 0x8c, 0));
    setColor(MinecartTracks, 0, getColor(PoweredRail));
    setColor(CobblestoneStairs, 0, color(120, 120, 120, 128));
    setColor(WallSign, 0, SharedInvisColor);
    setColor(Lever, 0, SharedInvisColor);
    setColor(StonePressurePlate, 0, color(120,120,120,255));
    setColor(IronDoor, 0, SharedInvisColor);
    setColor(WoodenPressurePlate, 0, SharedInvisColor);
    setColor(RedstoneOre, 0, color(143,125,125,0xff));
    setColor(GlowingRedstoneOre, 0, color(163,145,145,0xff));
    setColor(RedstoneTorchOff, 0, color(181,140,64,32),SharedInvisColor,false);
    setColor(RedstoneTorchOn, 0, color(255,0,0,0xb0), SharedInvisColor, false);
    setColor(StoneButton, 0, SharedInvisColor);
    setColor(Snow, 0, color(255, 255, 255, 255));
    setColor(Ice, 0, color(120, 120, 255, 120));
    setColor(SnowBlock, 0, color(255, 255, 255, 255));
    setColor(Cactus, 0, color(85,107,47,255));
    setColor(Clay, 0, color(0x90, 0x98, 0xa8, 0xff));
    setColor(Reed, 0, color(193,234,150,255));
    setColor(Jukebox, 0, color(0x7d, 0x42, 0x2c, 0xff));
    setColor(Fence, 0, color(0x58, 0x36, 0x16, 200));
    setColor(Pumpkin, 0, color(0xe3, 0x90, 0x1d, 0xff));
    setColor(Bloodstone, 0, color(0xc2, 0x73, 0x73, 0xff));
    setColor(Slowsand, 0, color(0x79, 0x61, 0x52, 0xff));
    setColor(Lightstone, 0, color(0xff, 0xbc, 0x5e, 0xff));
    setColor(Trapdoor, 0, getColor(WoodenPressurePlate));
    setColor(Portal, 0, color(0x3c, 0x0d, 0x6a, 0x7f));
    setColor(Jackolantern, 0, getColor(Pumpkin));
    setColor(RedstoneRepeaterOn, 0, getColor(RedstoneWire));
    setColor(RedstoneRepeaterOff, 0, getColor(RedstoneWire));
    setColor(Cake, 0, color(228,205,206,255));
    setColor(EggBlock, 0, getColor(Stone));
    setColor(StoneBrick, 0, getColor(Stone));
    setColor(HugeRedMushroom, 0, color(183,31,29,0xff));
    setColor(HugeBrownMushroom, 0, color(206,174,123,0xff));
    setColor(IronBars, 0, getColor(IronBlock));
    setColor(GlassPane, 0, getColor(Glass));
    setColor(Melon, 0, color(50,200,45,192));
    setColor(PumpkinStem, 0, color(0x00, 0x00, 0x00, 0x00));
    setColor(MelonStem, 0, color(0x00, 0x00, 0x00, 0x00));
    setColor(LilyPad, 0, color(50,89,45,128), color(50,89,45,128));
    setColor(NetherBrick, 0, color(66,32,38,255));
    setColor(NetherBrickFence, 0, color(66,32,38,200));
    setColor(NetherBrickStairs, 0, getColor(NetherBrick));
    setColor(NetherWart, 0, color(149,21,8,255));
    setColor(Mycelium, 0, color(110,93,133,255));
    setColor(Vines, 0, color(50,89,45,128), color(50,89,45,128));
    setColor(FenceGate, 0, getColor(Fence));
    setColor(BrickStairs, 0, getColor(Brick));
    setColor(StoneBrickStairs, 0, getColor(Stone));
    setColor(PineLeaves, 0, color(50,89,45,128));
    setColor(BirchLeaves, 0, color(94,167,84,128));

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
    MaterialModes[FenceGate] = Block;
    MaterialModes[BrickStairs] = Block;
    MaterialModes[StoneBrickStairs] = Block;
    MaterialModes[PineLeaves] = Block;
    MaterialModes[BirchLeaves] = Block;

    /* Special colors depending on data value
     * Start with the highest index to reduce allocation time complexity 
     * The order of the following entries does not matter.
     */
    setColor(Wool, WoolBlack, color(27, 23, 23, 255));
    setColor(Wool, WoolWhite, color(223, 223, 223, 255));
    setColor(Wool, WoolOrange, color(234, 128, 55, 255));
    setColor(Wool, WoolMagenta, color(191, 76, 201, 255));
    setColor(Wool, WoolLightBlue, color(105, 139, 212, 255));
    setColor(Wool, WoolYellow, color(195, 181, 28, 255));
    setColor(Wool, WoolLightGreen, color(59, 189, 48, 255));
    setColor(Wool, WoolPink, color(218, 132, 155, 255));
    setColor(Wool, WoolGray, color(67, 67, 67, 255));
    setColor(Wool, WoolLightGray, color(159, 166, 166, 255));
    setColor(Wool, WoolCyan, color(39, 117, 150, 255));
    setColor(Wool, WoolPurple, color(130, 54, 196, 255));
    setColor(Wool, WoolBlue, color(39, 51, 154, 255));
    setColor(Wool, WoolBrown, color(86, 51, 28, 255));
    setColor(Wool, WoolDarkGreen, color(56, 77, 24, 255));
    setColor(Wool, WoolRed, color(164, 45, 41, 255));

    setColor(Step, StepCobblestone, getColor(Cobblestone));
    setColor(Step, StepStone, getColor(Stone));
    setColor(Step, StepSandstone, getColor(Sandstone));
    setColor(Step, StepWood, getColor(Wood));

    setColor(DoubleStep, StepCobblestone, getColor(Cobblestone));
    setColor(DoubleStep, StepStone, getColor(Stone));
    setColor(DoubleStep, StepSandstone, getColor(Sandstone));
    setColor(DoubleStep, StepWood, getColor(Wood));
  }
  
  void deinitialize_constants() {
    delete [] MaterialColorData;
  }
  
  void setColor(int material, int idx, color top,
      color side, bool darken) {

    MaterialColorData[material].count =
      idx >= MaterialColorData[material].count ?
      1+idx : MaterialColorData[material].count;

    MaterialColorData[material].top[idx] = top;
    if (side == SharedInvisColor) {
      MaterialColorData[material].side[idx] = color(top);
    } else {
      MaterialColorData[material].side[idx] = color(side);
    }
    if (darken) {
      MaterialColorData[material].side[idx].darken(0x20);
    }
  }
}

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
    MaterialName[Cobweb] = "Cobweb";
    MaterialName[TallGrass] = "TallGrass";
    MaterialName[DeadShrub] = "DeadShrub";
    MaterialName[PistonBase] = "PistonBase";
    MaterialName[PistonPlatform] = "PistonPlatform";
    MaterialName[Wool] = "Wool";
    MaterialName[PistonMovedBlock] = "PistonMovedBlock";
    MaterialName[Dandelion] = "Dandelion";
    MaterialName[SmallFlowers] = "SmallFlowers";
    MaterialName[BrownMushroom] = "BrownMushroom";
    MaterialName[RedMushroom] = "RedMushroom";
    MaterialName[GoldBlock] = "GoldBlock";
    MaterialName[IronBlock] = "IronBlock";
    MaterialName[DoubleStoneSlab] = "DoubleStoneSlab";
    MaterialName[StoneSlab] = "StoneSlab";
    MaterialName[Brick] = "Brick";
    MaterialName[TNT] = "TNT";
    MaterialName[Bookcase] = "Bookcase";
    MaterialName[MossyCobblestone] = "MossyCobblestone";
    MaterialName[Obsidian] = "Obsidian";
    MaterialName[Torch] = "Torch";
    MaterialName[Fire] = "Fire";
    MaterialName[MobSpawner] = "MobSpawner";
    MaterialName[OakWoodStairs] = "OakWoodStairs";
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
    MaterialName[Portal] = "Portal";
    MaterialName[Jackolantern] = "Jackolantern";
    MaterialName[Cake] = "Cake";
    MaterialName[RedstoneRepeaterOn] = "RedstoneRepeaterOn";
    MaterialName[RedstoneRepeaterOff] = "RedstoneRepeaterOff";
    MaterialName[StainedGlass] = "StainedGlass";
    MaterialName[Trapdoor] = "Trapdoor";
    MaterialName[EggBlock] = "EggBlock";
    MaterialName[StoneBrick] = "StoneBrick";
    MaterialName[HugeBrownMushroom] = "HugeBrownMushroom";
    MaterialName[HugeRedMushroom] = "HugeRedMushroom";
    MaterialName[IronBars] = "IronBars";
    MaterialName[GlassPane] = "GlassPane";
    MaterialName[Melon] = "Melon";
    MaterialName[PumpkinStem] = "PumpkinStem";
    MaterialName[MelonStem] = "MelonStem";
    MaterialName[Vines] = "Vines";
    MaterialName[FenceGate] = "FenceGate";
    MaterialName[BrickStairs] = "BrickStairs";
    MaterialName[StoneBrickStairs] = "StoneBrickStairs";
    MaterialName[Mycelium] = "Mycelium";
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
    MaterialName[RedstoneLampOff] = "RedstoneLampOff";
    MaterialName[RedstoneLampOn] = "RedstoneLampOn";
    MaterialName[WoodenDoubleSlab] = "WoodenDoubleSlab";
    MaterialName[WoodenSlab] = "WoodenSlab";
    MaterialName[Cocoa] = "Cocoa";
    MaterialName[SandstoneStairs] = "SandstoneStairs";
    MaterialName[EmeraldOre] = "EmeraldOre";
    MaterialName[EmeraldChest] = "EmeraldChest";
    MaterialName[TripwireHook] = "TripwireHook";
    MaterialName[Tripwire] = "Tripwire";
    MaterialName[EmeraldBlock] = "EmeraldBlock";
    MaterialName[SpruceWoodStairs] = "SpruceWoodStairs";
    MaterialName[BirchWoodStairs] = "BirchWoodStairs";
    MaterialName[JungleWoodStairs] = "JungleWoodStairs";
    MaterialName[CommandBlock] = "CommandBlock";
    MaterialName[Beacon] = "Beacon";
    MaterialName[CobblestoneWall] = "CobblestoneWall";
    MaterialName[FlowerPot] = "FlowerPot";
    MaterialName[Carrots] = "Carrots";
    MaterialName[Potatoes] = "Potatoes";
    MaterialName[WoodenButton] = "WoodenButton";
    MaterialName[MobHead] = "MobHead";
    MaterialName[Anvil] = "Anvil";
    MaterialName[TrappedChest] = "TrappedChest";
    MaterialName[WeightedPressurePlateLight] = "WeightedPressurePlateLight";
    MaterialName[WeightedPressurePlateHeavy] = "WeightedPressurePlateHeavy";
    MaterialName[RedstoneComparatorOff] = "RedstoneComparatorOff";
    MaterialName[RedstoneComparatorOn] = "RedstoneComparatorOn";
    MaterialName[DaylightSensor] = "DaylightSensor";
    MaterialName[RedstoneBlock] = "RedstoneBlock";
    MaterialName[NetherQuartzOre] = "QuartzOre";
    MaterialName[Hopper] = "Hopper";
    MaterialName[QuartzBlock] = "QuartzBlock";
    MaterialName[QuartzStairs] = "QuartzStairs";
    MaterialName[ActivatorRail] = "ActivatorRail";
    MaterialName[Dropper] = "Dropper";
    MaterialName[StainedClay] = "StainedClay";
    MaterialName[StainedGlassPane] = "StainedGlassPane";
    MaterialName[Leaves2] = "Leaves2";
    MaterialName[Log2] = "Log2";
    MaterialName[AcaciaWoodStairs] = "AcaciaWoodStairs";
    MaterialName[DarkOakWoodStairs] = "DarkOakWoodStairs";
    //MaterialName[SlimeBlock] = "SlimeBlock";
    //MaterialName[Barrier] = "Barrier";
    //MaterialName[IronTrapdoor] = "IronTrapdoor";
    //MaterialName[UNUSED] = "UNUSED";
    //MaterialName[UNUSED] = "UNUSED";
    MaterialName[HayBlock] = "HayBlock";
    MaterialName[Carpet] = "Carpet";
    MaterialName[HardenedClay] = "HardenedClay";
    MaterialName[CoalBlock] = "CoalBlock";
    MaterialName[PackedIce] = "PackedIce";
    MaterialName[LargeFlowers] = "LargeFlowers";
    MaterialName[PineLeaves] = "PineLeaves";
    MaterialName[BirchLeaves] = "BirchLeaves";

    set_color(Air, 0, color(255,255,255,0), SharedInvisColor, false);
    set_color(Stone, 0, color(128,128,128,255));
    set_color(Grass, 0, color(120,172,70,255), color(134,96,67,255));
    set_color(Dirt, 0, get_side_color(Grass));
    set_color(Cobblestone, 0, color(100,100,100,255));
    set_color(Wood, 0, color(157,128,79,255));
    // NOTE: See below for Sapling
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
    set_color(Cobweb, 0, SharedInvisColor); // TODO
    set_color(TallGrass, 0,
      color(0x90, 0xbc, 0x27, 0xff), color(0x90, 0xbc, 0x27, 0xff));
    set_color(DeadShrub, 0, get_color(Wood));
    set_color(PistonBase, 0, get_color(Wood));
    set_color(PistonPlatform, 0, get_color(Air));
    // NOTE: See below for Wool
    set_color(PistonMovedBlock, 0, SharedInvisColor);
    set_color(Dandelion, 0, color(255,255,0,255));
    set_color(SmallFlowers, 0, color(255,0,0,255));
    set_color(BrownMushroom, 0, color(200, 200, 0, 255));
    set_color(RedMushroom, 0, color(255, 0, 0, 255));
    set_color(GoldBlock, 0, color(0xff, 0xed, 0x8c, 0xff));
    set_color(IronBlock, 0, color(0xd9, 0xd9, 0xd9, 0xff));
    // NOTE: See below for DoubleStoneSlab
    // NOTE: See below for StoneSlab
    set_color(Brick, 0, color(0x56, 0x23, 0x17, 0xff));
    set_color(TNT, 0, color(0xff, 0x0, 0x0, 0xff));
    set_color(Bookcase, 0, color(0xbf, 0xa9, 0x74, 0xff));
    set_color(MossyCobblestone, 0, color(0x7f, 0xae, 0x7d, 0xff));
    set_color(Obsidian, 0, color(0x11, 0x0d, 0x1a, 0xff));
    set_color(Torch, 0, color(0xff, 0xe1, 0x60,0xd0), SharedInvisColor, false);
    set_color(Fire, 0, color(0xe0, 0xae, 0x15, 0xff));
    set_color(MobSpawner, 0, color(0xff, 0xff, 0xff, 0x00));
    // NOTE: See below for OakWoodStairs
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
    set_color(Portal, 0, color(0x3c, 0x0d, 0x6a, 0x7f));
    set_color(Jackolantern, 0, get_color(Pumpkin));
    set_color(Cake, 0, color(228,205,206,255));
    set_color(RedstoneRepeaterOff, 0, get_color(RedstoneWire));
    set_color(RedstoneRepeaterOn, 0, get_color(RedstoneWire));
    // NOTE: See below for StainedGlass
    set_color(Trapdoor, 0, get_color(WoodenPressurePlate));
    set_color(EggBlock, 0, get_color(Stone));
    set_color(StoneBrick, 0, get_color(Stone));
    set_color(HugeBrownMushroom, 0, color(206,174,123,0xff));
    set_color(HugeRedMushroom, 0, color(183,31,29,0xff));
    set_color(IronBars, 0, get_color(IronBlock));
    set_color(GlassPane, 0, get_color(Glass));
    set_color(Melon, 0, color(50,200,45,192));
    set_color(PumpkinStem, 0, color(0x00, 0x00, 0x00, 0x00));
    set_color(MelonStem, 0, color(0x00, 0x00, 0x00, 0x00));
    set_color(Vines, 0, color(50,89,45,128), color(50,89,45,128));
    set_color(FenceGate, 0, get_color(Fence));
    set_color(BrickStairs, 0, get_color(Brick));
    set_color(StoneBrickStairs, 0, get_color(Stone));
    set_color(Mycelium, 0, color(110,93,133,255));
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
    set_color(RedstoneLampOff, 0, SharedInvisColor); // TODO
    set_color(RedstoneLampOn, 0, SharedInvisColor); // TODO
    set_color(WoodenDoubleSlab, 0, SharedInvisColor); // TODO
    set_color(WoodenSlab, 0, SharedInvisColor); // TODO
    set_color(Cocoa, 0, SharedInvisColor); // TODO
    set_color(SandstoneStairs, 0, SharedInvisColor); // TODO
    set_color(EmeraldOre, 0, SharedInvisColor); // TODO
    set_color(EmeraldChest, 0, SharedInvisColor); // TODO
    set_color(TripwireHook, 0, SharedInvisColor); // TODO
    set_color(Tripwire, 0, SharedInvisColor); // TODO
    set_color(EmeraldBlock, 0, SharedInvisColor); // TODO
    // NOTE: See below for SpruceWoodStairs
    // NOTE: See below for BirchWoodStairs
    // NOTE: See below for JungleWoodStairs
    set_color(CommandBlock, 0, SharedInvisColor); // TODO
    set_color(Beacon, 0, SharedInvisColor); // TODO
    set_color(CobblestoneWall, 0, SharedInvisColor); // TODO
    set_color(FlowerPot, 0, SharedInvisColor); // TODO
    set_color(Carrots, 0, SharedInvisColor); // TODO
    set_color(Potatoes, 0, SharedInvisColor); // TODO
    set_color(WoodenButton, 0, SharedInvisColor); // TODO
    set_color(MobHead, 0, SharedInvisColor); // TODO
    set_color(Anvil, 0, SharedInvisColor); // TODO
    set_color(TrappedChest, 0, SharedInvisColor); // TODO
    set_color(WeightedPressurePlateLight, 0, SharedInvisColor); // TODO
    set_color(WeightedPressurePlateHeavy, 0, SharedInvisColor); // TODO
    set_color(RedstoneComparatorOff, 0, SharedInvisColor); // TODO
    set_color(RedstoneComparatorOn, 0, SharedInvisColor); // TODO
    set_color(DaylightSensor, 0, SharedInvisColor); // TODO
    set_color(RedstoneBlock, 0, SharedInvisColor); // TODO
    set_color(NetherQuartzOre, 0, SharedInvisColor); // TODO
    set_color(Hopper, 0, SharedInvisColor); // TODO
    set_color(QuartzBlock, 0, SharedInvisColor); // TODO
    set_color(QuartzStairs, 0, SharedInvisColor); // TODO
    set_color(ActivatorRail, 0, SharedInvisColor); // TODO
    set_color(Dropper, 0, SharedInvisColor); // TODO
    // NOTE: See below for StainedClay
    // NOTE: See below for StainedGlassPane
    set_color(Leaves2, 0, SharedInvisColor); // TODO
    set_color(Log2, 0, SharedInvisColor); // TODO
    // NOTE: See below for AcaciaWoodStairs
    // NOTE: See below for DarkOakWoodStairs
    //set_color(SlimeBlock, 0, SharedInvisColor); // TODO
    //set_color(Barrier, 0, SharedInvisColor); // TODO
    //set_color(IronTrapdoor, 0, SharedInvisColor); // TODO
    //set_color(UNUSED, 0, SharedInvisColor); // TODO
    //set_color(UNUSED, 0, SharedInvisColor); // TODO
    set_color(HayBlock, 0, SharedInvisColor); // TODO
    set_color(HardenedClay, 0, SharedInvisColor); // TODO
    set_color(CoalBlock, 0, SharedInvisColor); // TODO
    set_color(PackedIce, 0, SharedInvisColor); // TODO
    set_color(LargeFlowers, 0, SharedInvisColor); // TODO
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
    MaterialModes[Cobweb] = Block;
    MaterialModes[TallGrass] = Block;
    MaterialModes[DeadShrub] = TorchBlock;
    MaterialModes[PistonBase] = Block;
    MaterialModes[PistonPlatform] = Block;
    MaterialModes[Wool] = Block;
    MaterialModes[PistonMovedBlock] = Block;
    MaterialModes[Dandelion] = Block;
    MaterialModes[SmallFlowers] = Block;
    MaterialModes[BrownMushroom] = Block;
    MaterialModes[RedMushroom] = Block;
    MaterialModes[GoldBlock] = Block;
    MaterialModes[IronBlock] = Block;
    MaterialModes[DoubleStoneSlab] = Block;
    MaterialModes[StoneSlab] = HalfBlock;
    MaterialModes[Brick] = Block;
    MaterialModes[TNT] = Block;
    MaterialModes[Bookcase] = Block;
    MaterialModes[MossyCobblestone] = Block;
    MaterialModes[Obsidian] = Block;
    MaterialModes[Torch] = TorchBlock;
    MaterialModes[Fire] = Block;
    MaterialModes[MobSpawner] = Block;
    MaterialModes[OakWoodStairs] = Block;
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
    MaterialModes[Portal] = Block;
    MaterialModes[Jackolantern] = Block;
    MaterialModes[Cake] = HalfBlock;
    MaterialModes[RedstoneRepeaterOff] = Block;
    MaterialModes[RedstoneRepeaterOn] = Block;
    MaterialModes[StainedGlass] = Block;
    MaterialModes[Trapdoor] = HalfBlock;
    MaterialModes[EggBlock] = Block;
    MaterialModes[StoneBrick] = Block;
    MaterialModes[HugeBrownMushroom] = Block;
    MaterialModes[HugeRedMushroom] = Block;
    MaterialModes[IronBars] = Block;
    MaterialModes[GlassPane] = Block;
    MaterialModes[Melon] = Block;
    MaterialModes[PumpkinStem] = Block;
    MaterialModes[MelonStem] = Block;
    MaterialModes[Vines] = Block;
    MaterialModes[FenceGate] = Block;
    MaterialModes[BrickStairs] = Block;
    MaterialModes[StoneBrickStairs] = Block;
    MaterialModes[Mycelium] = Block;
    MaterialModes[LilyPad] = Block; 
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
    MaterialModes[RedstoneLampOff] = Block;
    MaterialModes[RedstoneLampOn] = Block;
    MaterialModes[WoodenDoubleSlab] = Block;
    MaterialModes[WoodenSlab] = HalfBlock;
    MaterialModes[Cocoa] = Block; // TODO
    MaterialModes[SandstoneStairs] = Block;
    MaterialModes[EmeraldOre] = Block;
    MaterialModes[EmeraldChest] = Block;
    MaterialModes[TripwireHook] = Block;
    MaterialModes[Tripwire] = HalfBlock; // TODO
    MaterialModes[EmeraldBlock] = Block;
    MaterialModes[SpruceWoodStairs] = Block;
    MaterialModes[BirchWoodStairs] = Block;
    MaterialModes[JungleWoodStairs] = Block;
    MaterialModes[CommandBlock] = Block;
    MaterialModes[Beacon] = Block;
    MaterialModes[CobblestoneWall] = Block;
    MaterialModes[FlowerPot] = Block; // TODO
    MaterialModes[Carrots] = Block;
    MaterialModes[Potatoes] = Block;
    MaterialModes[WoodenButton] = Block;
    MaterialModes[MobHead] = Block;
    MaterialModes[Anvil] = Block;
    MaterialModes[TrappedChest] = Block;
    MaterialModes[WeightedPressurePlateLight] = HalfBlock;
    MaterialModes[WeightedPressurePlateHeavy] = HalfBlock;
    MaterialModes[RedstoneComparatorOff] = Block;
    MaterialModes[RedstoneComparatorOn] = Block;
    MaterialModes[DaylightSensor] = HalfBlock; // TODO
    MaterialModes[RedstoneBlock] = Block;
    MaterialModes[NetherQuartzOre] = Block;
    MaterialModes[Hopper] = Block;
    MaterialModes[QuartzBlock] = Block;
    MaterialModes[QuartzStairs] = Block;
    MaterialModes[ActivatorRail] = Block;
    MaterialModes[Dropper] = Block;
    MaterialModes[StainedClay] = Block;
    MaterialModes[StainedGlassPane] = Block;
    MaterialModes[Leaves2] = Block;
    MaterialModes[Log2] = Block;
    MaterialModes[AcaciaWoodStairs] = Block;
    MaterialModes[DarkOakWoodStairs] = Block;
    //MaterialModes[SlimeBlock] = Block;
    //MaterialModes[Barrier] = Block;
    //MaterialModes[IronTrapdoor] = HalfBlock;
    //MaterialModes[UNUSED] = Block;
    //MaterialModes[UNUSED] = Block;
    MaterialModes[HayBlock] = Block;
    MaterialModes[Carpet] = HalfBlock;
    MaterialModes[HardenedClay] = Block;
    MaterialModes[CoalBlock] = Block;
    MaterialModes[PackedIce] = Block;
    MaterialModes[LargeFlowers] = Block;
    MaterialModes[PineLeaves] = Block;
    MaterialModes[BirchLeaves] = Block;

    /* 
     * Special colors depending on data value
     * Start with the highest index to reduce allocation time complexity 
     * The order of the following entries does not matter.
     */
    // Dirt (0x03)
    // TODO Use different colors?
    set_color(Dirt, DirtPodzol, color(134, 96, 67, 255));
    set_color(Dirt, DirtNormal, color(134, 96, 67, 255));
    set_color(Dirt, DirtCoarse, color(134, 96, 67, 255));

    // Wood (0x05)
    // TODO Use different colors?
    set_color(Wood, WoodDarkOak, color(157,128,79,255));
    set_color(Wood, WoodOak, color(157,128,79,255));
    set_color(Wood, WoodSpruce, color(157,128,79,255));
    set_color(Wood, WoodBirch, color(157,128,79,255));
    set_color(Wood, WoodJungle, color(157,128,79,255));
    set_color(Wood, WoodAcacia, color(157,128,79,255));

    // The stairs dependent on the wood color.
    set_color(SpruceWoodStairs, 0, get_color(Wood, WoodSpruce));
    set_color(OakWoodStairs, 0, get_color(Wood, WoodOak));
    set_color(BirchWoodStairs, 0, get_color(Wood, WoodBirch));
    set_color(JungleWoodStairs, 0, get_color(Wood, WoodJungle));
    set_color(AcaciaWoodStairs, 0, get_color(Wood, WoodAcacia));
    set_color(DarkOakWoodStairs, 0, get_color(Wood, WoodDarkOak));

    // Sapling (0x06)
    // TODO Use different colors?
    set_color(Sapling, WoodDarkOak, color(120,120,120,0));
    set_color(Sapling, WoodOak, color(120,120,120,0));
    set_color(Sapling, WoodSpruce, color(120,120,120,0));
    set_color(Sapling, WoodBirch, color(120,120,120,0));
    set_color(Sapling, WoodJungle, color(120,120,120,0));
    set_color(Sapling, WoodAcacia, color(120,120,120,0));

    // Sand (0x0C)
    set_color(Sand, SandRed, color(186, 102, 44, 255));
    set_color(Sand, SandNormal, color(218, 210, 158, 255));

    // Log (0x11)
    // TODO Use different colors?
    set_color(Log, LogJungleBark, color(102,81,51,255));
    set_color(Log, LogOakBark, color(102,81,51,255));
    set_color(Log, LogSpruceBark, color(102,81,51,255));
    set_color(Log, LogBirchBark, color(102,81,51,255));
    set_color(Log, LogOakUp, color(102,81,51,255),  get_color(Log, LogOakBark));
    set_color(Log, LogSpruceUp, color(102,81,51,255), get_color(Log, LogSpruceBark));
    set_color(Log, LogBirchUp, color(102,81,51,255), get_color(Log, LogBirchBark));
    set_color(Log, LogJungleUp, color(102,81,51,255), get_color(Log, LogJungleBark));
    set_color(Log, LogOakEast, get_side_color(Log, LogOakUp), get_color(Log, LogOakUp));
    set_color(Log, LogSpruceEast, get_side_color(Log, LogSpruceUp), get_color(Log, LogSpruceUp));
    set_color(Log, LogBirchEast, get_side_color(Log, LogBirchUp), get_color(Log, LogBirchUp));
    set_color(Log, LogJungleEast, get_side_color(Log, LogJungleUp), get_color(Log, LogJungleUp));
    set_color(Log, LogOakNorth, get_side_color(Log, LogOakUp), get_color(Log, LogOakUp));
    set_color(Log, LogSpruceNorth, get_side_color(Log, LogSpruceUp), get_color(Log, LogSpruceUp));
    set_color(Log, LogBirchNorth, get_side_color(Log, LogBirchUp), get_color(Log, LogBirchUp));
    set_color(Log, LogJungleNorth, get_side_color(Log, LogJungleUp), get_color(Log, LogJungleUp));

    // Wool (0x23)
    set_color(Wool, ColorBlack, color(27, 23, 23, 255));
    set_color(Wool, ColorWhite, color(223, 223, 223, 255));
    set_color(Wool, ColorOrange, color(234, 128, 55, 255));
    set_color(Wool, ColorMagenta, color(191, 76, 201, 255));
    set_color(Wool, ColorLightBlue, color(105, 139, 212, 255));
    set_color(Wool, ColorYellow, color(195, 181, 28, 255));
    set_color(Wool, ColorLightGreen, color(59, 189, 48, 255));
    set_color(Wool, ColorPink, color(218, 132, 155, 255));
    set_color(Wool, ColorGray, color(67, 67, 67, 255));
    set_color(Wool, ColorLightGray, color(159, 166, 166, 255));
    set_color(Wool, ColorCyan, color(39, 117, 150, 255));
    set_color(Wool, ColorPurple, color(130, 54, 196, 255));
    set_color(Wool, ColorBlue, color(39, 51, 154, 255));
    set_color(Wool, ColorBrown, color(86, 51, 28, 255));
    set_color(Wool, ColorDarkGreen, color(56, 77, 24, 255));
    set_color(Wool, ColorRed, color(164, 45, 41, 255));

    // DoubleStoneSlab (0x2B)
    set_color(DoubleStoneSlab, StepCobblestone, get_color(Cobblestone));
    set_color(DoubleStoneSlab, StepStone, get_color(Stone));
    set_color(DoubleStoneSlab, StepSandstone, get_color(Sandstone));
    set_color(DoubleStoneSlab, StepWood, get_color(Wood));

    // StoneSlab (0x2C)
    set_color(StoneSlab, StepCobblestone, get_color(Cobblestone));
    set_color(StoneSlab, StepStone, get_color(Stone));
    set_color(StoneSlab, StepSandstone, get_color(Sandstone));
    set_color(StoneSlab, StepWood, get_color(Wood));

    // StainedGlass (0x5F)
    set_color(StainedGlass, ColorBlack, color(27, 23, 23, 255));
    set_color(StainedGlass, ColorWhite, color(223, 223, 223, 255));
    set_color(StainedGlass, ColorOrange, color(234, 128, 55, 255));
    set_color(StainedGlass, ColorMagenta, color(191, 76, 201, 255));
    set_color(StainedGlass, ColorLightBlue, color(105, 139, 212, 255));
    set_color(StainedGlass, ColorYellow, color(195, 181, 28, 255));
    set_color(StainedGlass, ColorLightGreen, color(59, 189, 48, 255));
    set_color(StainedGlass, ColorPink, color(218, 132, 155, 255));
    set_color(StainedGlass, ColorGray, color(67, 67, 67, 255));
    set_color(StainedGlass, ColorLightGray, color(159, 166, 166, 255));
    set_color(StainedGlass, ColorCyan, color(39, 117, 150, 255));
    set_color(StainedGlass, ColorPurple, color(130, 54, 196, 255));
    set_color(StainedGlass, ColorBlue, color(39, 51, 154, 255));
    set_color(StainedGlass, ColorBrown, color(86, 51, 28, 255));
    set_color(StainedGlass, ColorDarkGreen, color(56, 77, 24, 255));
    set_color(StainedGlass, ColorRed, color(164, 45, 41, 255));

    // StainedClay (0x9F)
    set_color(StainedClay, ColorBlack, color(39, 24, 17, 255));
    set_color(StainedClay, ColorWhite, color(210, 177, 161, 255));
    set_color(StainedClay, ColorOrange, color(162, 82, 36, 255));
    set_color(StainedClay, ColorMagenta, color(149, 89, 110, 255));
    set_color(StainedClay, ColorLightBlue, color(117, 111, 140, 255));
    set_color(StainedClay, ColorYellow, color(186, 134, 36, 255));
    set_color(StainedClay, ColorLightGreen, color(102, 115, 48, 255));
    set_color(StainedClay, ColorPink, color(163, 80, 80, 255));
    set_color(StainedClay, ColorGray, color(57, 42, 35, 255));
    set_color(StainedClay, ColorLightGray, color(135, 107, 98, 255));
    set_color(StainedClay, ColorCyan, color(91, 95, 94, 255));
    set_color(StainedClay, ColorPurple, color(119, 72, 87, 255));
    set_color(StainedClay, ColorBlue, color(76, 62, 92, 255));
    set_color(StainedClay, ColorBrown, color(77, 51, 36, 255));
    set_color(StainedClay, ColorDarkGreen, color(76, 84, 43, 255));
    set_color(StainedClay, ColorRed, color(142, 61, 47, 255));

    // StainedGlassPane (0xA0)
    set_color(StainedGlassPane, ColorBlack, get_color(StainedGlass, ColorBlack));
    set_color(StainedGlassPane, ColorWhite, get_color(StainedGlass, ColorWhite));
    set_color(StainedGlassPane, ColorOrange, get_color(StainedGlass, ColorOrange));
    set_color(StainedGlassPane, ColorMagenta, get_color(StainedGlass, ColorMagenta));
    set_color(StainedGlassPane, ColorLightBlue, get_color(StainedGlass, ColorLightBlue));
    set_color(StainedGlassPane, ColorYellow, get_color(StainedGlass, ColorYellow));
    set_color(StainedGlassPane, ColorLightGreen, get_color(StainedGlass, ColorLightGreen));
    set_color(StainedGlassPane, ColorPink, get_color(StainedGlass, ColorPink));
    set_color(StainedGlassPane, ColorGray, get_color(StainedGlass, ColorGray));
    set_color(StainedGlassPane, ColorLightGray, get_color(StainedGlass, ColorLightGray));
    set_color(StainedGlassPane, ColorCyan, get_color(StainedGlass, ColorCyan));
    set_color(StainedGlassPane, ColorPurple, get_color(StainedGlass, ColorPurple));
    set_color(StainedGlassPane, ColorBlue, get_color(StainedGlass, ColorBlue));
    set_color(StainedGlassPane, ColorBrown, get_color(StainedGlass, ColorBrown));
    set_color(StainedGlassPane, ColorDarkGreen, get_color(StainedGlass, ColorDarkGreen));
    set_color(StainedGlassPane, ColorRed, get_color(StainedGlass, ColorRed));

    // Log2 (0xA2)
    // TODO Use different colors?
    set_color(Log2, Log2DarkWoodBark, get_color(Log, LogOakBark));
    set_color(Log2, Log2AcaciaBark, get_color(Log, LogOakBark));
    set_color(Log2, Log2AcaciaUp, get_color(Log, LogOakUp), get_color(Log2, Log2AcaciaBark));
    set_color(Log2, Log2DarkWoodUp, get_color(Log, LogOakUp), get_color(Log2, Log2DarkWoodBark));
    set_color(Log2, Log2AcaciaEast, get_color(Log2, Log2AcaciaBark), get_color(Log2, Log2AcaciaUp));
    set_color(Log2, Log2DarkWoodEast, get_color(Log2, Log2DarkWoodBark), get_color(Log2, Log2DarkWoodUp));
    set_color(Log2, Log2AcaciaNorth, get_color(Log2, Log2AcaciaBark), get_color(Log2, Log2AcaciaUp));
    set_color(Log2, Log2DarkWoodNorth, get_color(Log2, Log2DarkWoodBark), get_color(Log2, Log2DarkWoodUp));

    // Carpet (0xAB)
    set_color(Carpet, ColorBlack, get_color(Wool, ColorBlack));
    set_color(Carpet, ColorWhite, get_color(Wool, ColorWhite));
    set_color(Carpet, ColorOrange, get_color(Wool, ColorOrange));
    set_color(Carpet, ColorMagenta, get_color(Wool, ColorMagenta));
    set_color(Carpet, ColorLightBlue, get_color(Wool, ColorLightBlue));
    set_color(Carpet, ColorYellow, get_color(Wool, ColorYellow));
    set_color(Carpet, ColorLightGreen, get_color(Wool, ColorLightGreen));
    set_color(Carpet, ColorPink, get_color(Wool, ColorPink));
    set_color(Carpet, ColorGray, get_color(Wool, ColorGray));
    set_color(Carpet, ColorLightGray, get_color(Wool, ColorLightGray));
    set_color(Carpet, ColorCyan, get_color(Wool, ColorCyan));
    set_color(Carpet, ColorPurple, get_color(Wool, ColorPurple));
    set_color(Carpet, ColorBlue, get_color(Wool, ColorBlue));
    set_color(Carpet, ColorBrown, get_color(Wool, ColorBrown));
    set_color(Carpet, ColorDarkGreen, get_color(Wool, ColorDarkGreen));
    set_color(Carpet, ColorRed, get_color(Wool, ColorRed));
  }
  
  void deinitialize_constants() {
    delete [] MaterialColorData;
  }
}

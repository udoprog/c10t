// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "mc/level.hpp"

#include "nbt/nbt.hpp"
#include "mc/region.hpp"
#include "mc/level_info.hpp"

#include <boost/foreach.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

/* Legacy compound structure; For versions < 1.13
Compound() {
  Compound(Level) {
    List(Entities, TAG_Byte, 0): [ ]
    ByteArray(Biomes): (256 bytes)
    Long(LastUpdate): 7446079
    Int(xPos): -1
    Int(zPos): -1
    List(TileEntities, TAG_Compound, 0): [ ]
    Byte(TerrainPopulated): 0x1
    IntArray(HeightMap): (256 ints)
    List(Sections, TAG_Compound, 1): [
      Compound() {
        ByteArray(Data): (2048 bytes)
        ByteArray(SkyLight): (2048 bytes)
        ByteArray(BlockLight): (2048 bytes)
        Byte(Y): 0x0
        ByteArray(Blocks): (4096 bytes)
      }
    ]
  }
  // Beta 1.3 (19132) added a version tag.
  Int(DataVersion): 1343
}
*/

/* Versions >= 1.13
Compound() {
  Compound(Level) {
    String(Status): full
    Int(zPos): -32
    Long(LastUpdate): 7452163
    IntArray(Biomes): (1024 ints)
    Long(InhabitedTime): 1112
    Int(xPos): -32
    Compound(Heightmaps) {
      LongArray(OCEAN_FLOOR): (37 longs)
      LongArray(MOTION_BLOCKING_NO_LEAVES): (37 longs)
      LongArray(MOTION_BLOCKING): (37 longs)
      LongArray(WORLD_SURFACE): (37 longs)
    }
    List(TileEntities, TAG_End, 0): [ ]
    List(Entities, TAG_Compound, 1): [ ]
    Byte(isLightOn): 0x1
    List(TileTicks, TAG_End, 0): [ ]
    List(Sections, TAG_Compound, 9): [
      Compound() {
        Byte(Y): 0xffffffff
      }
      Compound() {
        LongArray(BlockStates): (342 longs)
        List(Palette, TAG_Compound, 22): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            String(Name): minecraft:bedrock
          }
          Compound() {
            String(Name): minecraft:stone
          }
          Compound() {
            String(Name): minecraft:gravel
          }
          Compound() {
            String(Name): minecraft:coal_ore
          }
          Compound() {
            String(Name): minecraft:granite
          }
          Compound() {
            Compound(Properties) {
              String(lit): false
            }
            String(Name): minecraft:redstone_ore
          }
          Compound() {
            Compound(Properties) {
              String(level): 0
            }
            String(Name): minecraft:lava
          }
          Compound() {
            String(Name): minecraft:iron_ore
          }
          Compound() {
            String(Name): minecraft:andesite
          }
          Compound() {
            String(Name): minecraft:emerald_ore
          }
          Compound() {
            String(Name): minecraft:cave_air
          }
          Compound() {
            Compound(Properties) {
              String(shape): north_south
            }
            String(Name): minecraft:rail
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): true
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(east): true
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            String(Name): minecraft:diorite
          }
          Compound() {
            String(Name): minecraft:diamond_ore
          }
          Compound() {
            String(Name): minecraft:oak_planks
          }
          Compound() {
            String(Name): minecraft:cobweb
          }
          Compound() {
            String(Name): minecraft:dirt
          }
          Compound() {
            String(Name): minecraft:lapis_ore
          }
          Compound() {
            String(Name): minecraft:infested_stone
          }
        ]
        Byte(Y): 0x0
        ByteArray(BlockLight): (2048 bytes)
      }
      Compound() {
        LongArray(BlockStates): (342 longs)
        List(Palette, TAG_Compound, 21): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            String(Name): minecraft:stone
          }
          Compound() {
            String(Name): minecraft:andesite
          }
          Compound() {
            String(Name): minecraft:diorite
          }
          Compound() {
            String(Name): minecraft:coal_ore
          }
          Compound() {
            String(Name): minecraft:dirt
          }
          Compound() {
            String(Name): minecraft:infested_stone
          }
          Compound() {
            String(Name): minecraft:emerald_ore
          }
          Compound() {
            String(Name): minecraft:gold_ore
          }
          Compound() {
            String(Name): minecraft:iron_ore
          }
          Compound() {
            String(Name): minecraft:granite
          }
          Compound() {
            String(Name): minecraft:gravel
          }
          Compound() {
            Compound(Properties) {
              String(level): 0
            }
            String(Name): minecraft:water
          }
          Compound() {
            String(Name): minecraft:cave_air
          }
          Compound() {
            String(Name): minecraft:oak_planks
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): true
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): true
              String(north): false
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(shape): north_south
            }
            String(Name): minecraft:rail
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): true
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            String(Name): minecraft:cobweb
          }
        ]
        Byte(Y): 0x1
        ByteArray(BlockLight): (2048 bytes)
      }
      Compound() {
        LongArray(BlockStates): (342 longs)
        List(Palette, TAG_Compound, 18): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            String(Name): minecraft:cave_air
          }
          Compound() {
            String(Name): minecraft:stone
          }
          Compound() {
            String(Name): minecraft:oak_planks
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): true
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): true
              String(north): false
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            String(Name): minecraft:granite
          }
          Compound() {
            Compound(Properties) {
              String(east): false
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): true
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            Compound(Properties) {
              String(east): true
              String(waterlogged): false
              String(south): false
              String(north): false
              String(west): false
            }
            String(Name): minecraft:oak_fence
          }
          Compound() {
            String(Name): minecraft:iron_ore
          }
          Compound() {
            String(Name): minecraft:dirt
          }
          Compound() {
            String(Name): minecraft:gravel
          }
          Compound() {
            String(Name): minecraft:cobweb
          }
          Compound() {
            String(Name): minecraft:infested_stone
          }
          Compound() {
            String(Name): minecraft:andesite
          }
          Compound() {
            String(Name): minecraft:coal_ore
          }
          Compound() {
            String(Name): minecraft:diorite
          }
        ]
        Byte(Y): 0x2
        ByteArray(BlockLight): (2048 bytes)
      }
      Compound() {
        LongArray(BlockStates): (256 longs)
        List(Palette, TAG_Compound, 11): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            String(Name): minecraft:stone
          }
          Compound() {
            String(Name): minecraft:infested_stone
          }
          Compound() {
            String(Name): minecraft:cave_air
          }
          Compound() {
            String(Name): minecraft:iron_ore
          }
          Compound() {
            String(Name): minecraft:dirt
          }
          Compound() {
            String(Name): minecraft:granite
          }
          Compound() {
            String(Name): minecraft:coal_ore
          }
          Compound() {
            String(Name): minecraft:andesite
          }
          Compound() {
            String(Name): minecraft:gravel
          }
          Compound() {
            String(Name): minecraft:diorite
          }
        ]
        Byte(Y): 0x3
      }
      Compound() {
        LongArray(BlockStates): (256 longs)
        List(Palette, TAG_Compound, 8): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            String(Name): minecraft:andesite
          }
          Compound() {
            String(Name): minecraft:stone
          }
          Compound() {
            String(Name): minecraft:granite
          }
          Compound() {
            String(Name): minecraft:diorite
          }
          Compound() {
            String(Name): minecraft:coal_ore
          }
          Compound() {
            String(Name): minecraft:dirt
          }
          Compound() {
            String(Name): minecraft:cave_air
          }
        ]
        Byte(Y): 0x4
      }
      Compound() {
        LongArray(BlockStates): (256 longs)
        List(Palette, TAG_Compound, 8): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            String(Name): minecraft:stone
          }
          Compound() {
            String(Name): minecraft:coal_ore
          }
          Compound() {
            String(Name): minecraft:dirt
          }
          Compound() {
            Compound(Properties) {
              String(snowy): false
            }
            String(Name): minecraft:grass_block
          }
          Compound() {
            String(Name): minecraft:dandelion
          }
          Compound() {
            Compound(Properties) {
              String(snowy): true
            }
            String(Name): minecraft:grass_block
          }
          Compound() {
            Compound(Properties) {
              String(layers): 1
            }
            String(Name): minecraft:snow
          }
        ]
        ByteArray(SkyLight): (2048 bytes)
        Byte(Y): 0x5
      }
      Compound() {
        LongArray(BlockStates): (256 longs)
        List(Palette, TAG_Compound, 2): [
          Compound() {
            String(Name): minecraft:air
          }
          Compound() {
            Compound(Properties) {
              String(layers): 1
            }
            String(Name): minecraft:snow
          }
        ]
        ByteArray(SkyLight): (2048 bytes)
        Byte(Y): 0x6
      }
      Compound() {
        ByteArray(SkyLight): (2048 bytes)
        Byte(Y): 0x7
      }
    ]
    List(PostProcessing, TAG_List, 10): [ ]
    Compound(Structures) { }
    List(LiquidTicks, TAG_End, 0): [ ]
  }
  Int(DataVersion): 2580
}
*/

namespace mc {
  enum section_name {
    Level,
    Sections,
    Palette,
    Properties,
    None
  };

  struct context_section {
    // shared
    nbt::Byte Y;
    boost::shared_ptr<nbt::ByteArray> SkyLight;

    // legacy
    boost::shared_ptr<nbt::ByteArray> Blocks;
    boost::shared_ptr<nbt::ByteArray> Data;
    boost::shared_ptr<nbt::ByteArray> BlockLight;

    // modern
    boost::shared_ptr<nbt::LongArray> BlockStates;
    std::vector<std::string> Palette;
    std::vector<std::map<std::string, std::string>> PaletteProperties;

    // Constructor for emplacing
    context_section() {
      // Minecraft seems to keep an
      // invalid entry first; which
      // has Y set to -1, so default
      // to -1 for invalid here as well.
      this->Y = -1;
    }
  };

  struct level_context {
    boost::shared_ptr<Level_Compound> Level;

    bool error;
    size_t error_where;
    const char* error_why;

    nbt::Int Version;
    std::vector<context_section> sections;

    section_name p[64];
    int pos;

    level_context() : error(false), error_where(0), error_why("")
    {
      this->Version = -1;
      this->Level.reset(new Level_Compound);
      this->pos = 0;
    }
  };

  Modern_Section_Compound::Modern_Section_Compound(
    nbt::Byte Y,
    boost::shared_ptr<nbt::LongArray> BlockStates,
    boost::shared_ptr<nbt::ByteArray> SkyLight,
    std::vector<std::string> &Palette,
    std::vector<std::map<std::string, std::string>> &PaletteProperties
  ) : Section_Compound(Y, SkyLight), BlockStates(BlockStates) {
    palette_size = Palette.size() < PaletteProperties.size() ? Palette.size() : PaletteProperties.size();
    BlockPalette.reset(new boost::optional<mc::BlockT>[palette_size]);
    for(size_t i = 0; i < palette_size; i++) {
      std::map<std::string, mc::MaterialT*>::iterator material_it = mc::MaterialMap.find(Palette[i]);
      if (material_it == mc::MaterialMap.end()) {
        std::cout << "unknown material: " << Palette[i] << std::endl;
        BlockPalette[i] = boost::optional<mc::BlockT>();
      } else {
        BlockT block;
        block.material = material_it->second;

        std::map<std::string, std::string>::iterator property_it;
        switch (block.material->mode) {
        case mc::MaterialMode::LargeFlowerBlock:
          property_it = PaletteProperties[i].find("half");
          if (property_it == PaletteProperties[i].end()) {
            block.properties.is_top = false;
          } else {
            // "lower" or "upper"
            block.properties.is_top = property_it->second.compare("upper") == 0;
          }
          break;
        case mc::MaterialMode::LogBlock:
          property_it = PaletteProperties[i].find("axis");
          if (property_it == PaletteProperties[i].end()) {
            block.properties.orientation = mc::BlockOrientation::UpDown;
          } else {
            // "x", "y" or "z"
            if (property_it->second.compare("x") == 0) {
              block.properties.orientation = mc::BlockOrientation::EastWest;
            } else if (property_it->second.compare("y") == 0) {
              block.properties.orientation = mc::BlockOrientation::UpDown;
            } else if (property_it->second.compare("z") == 0) {
              block.properties.orientation = mc::BlockOrientation::NorthSouth;
            } else {
              block.properties.orientation = mc::BlockOrientation::Invalid;
            }
          }
          break;
        }

        /* Debug properties
        std::pair<std::string, std::string> map_pair;
        BOOST_FOREACH(map_pair, PaletteProperties[i]) {
          std::cout << ">> " << map_pair.first << " " << map_pair.second << std::endl;
        }
        */

        BlockPalette[i] = boost::optional<BlockT>(block);
      }
    }

    // check bit packing method of array; check if the array fits exactly 4096 indices for
    // bit counts that don't divide 64 (size of long) then it is an optimal bit stream rather
    // then the expected truncated bit-packing. (as for why it is sometimes used I have no idea)
    if (palette_size > 16 && palette_size <= 32 && BlockStates->length == 320) {
      stream_bit_packing = true;
    } else if(palette_size > 32 && palette_size <= 64 && BlockStates->length == 384) {
      stream_bit_packing = true;
    } else if(palette_size > 64 && palette_size <= 128 && BlockStates->length == 448) {
      stream_bit_packing = true;
    } else if(palette_size > 256 && palette_size <= 512 && BlockStates->length == 576) {
      stream_bit_packing = true;
    } else if(palette_size > 512 && palette_size <= 1024 && BlockStates->length == 640) {
      stream_bit_packing = true;
    } else if(palette_size > 1024 && palette_size <= 2048 && BlockStates->length == 704) {
      stream_bit_packing = true;
    } else if(palette_size > 2048 && palette_size <= 4096 && BlockStates->length == 768) {
      stream_bit_packing = true;
    } else {
      // slack bits, if any, are assumed to be unused.
      stream_bit_packing = false;
    }
  }

  inline bool in_level_section(level_context* C) {
    return   C->pos == 4
          && C->p[0] == section_name::None
          && C->p[1] == section_name::Level
          && C->p[2] == section_name::Sections
          && C->p[3] == section_name::None;
  }

  inline bool in_palette_section(level_context* C) {
    return   C->pos == 6
          && C->p[0] == section_name::None
          && C->p[1] == section_name::Level
          && C->p[2] == section_name::Sections
          && C->p[3] == section_name::None
          && C->p[4] == section_name::Palette
          && C->p[5] == section_name::None;
  }

  inline bool in_palette_properties(level_context* C) {
    return   C->pos == 7
          && C->p[0] == section_name::None
          && C->p[1] == section_name::Level
          && C->p[2] == section_name::Sections
          && C->p[3] == section_name::None
          && C->p[4] == section_name::Palette
          && C->p[5] == section_name::None
          && C->p[6] == section_name::Properties;
  }

  void begin_compound(level_context* C, nbt::String name) {
    if (name.compare("Level") == 0) {
      C->p[C->pos++] = section_name::Level;
      return;
    } else if(in_palette_section(C)) {
      if(name.compare("Properties") == 0) {
        C->p[C->pos++] = section_name::Properties;
        return;
      }
    }

    C->p[C->pos++] = section_name::None;

    if (in_level_section(C)) {
      C->sections.emplace_back();
    } else if(in_palette_section(C)) {
      C->sections.back().PaletteProperties.emplace_back();
    }
  }

  void end_compound(level_context* C, nbt::String name) {
    --C->pos;
    // Construct level objects after parsing all data from region
    // file to ensure that version information is available.
    if (C->pos == 0) {
      BOOST_FOREACH(context_section &ctx_section, C->sections) {

        if (ctx_section.Y < 0) {
          // Skip invalid height indexes; these are typically 0 - 15.
          continue;
        }

        // Legacy or no version
        if (C->Version < 1519) {
          if (!ctx_section.Data) {
            std::cout << "missing Data" << std::endl;
          }

          if (!ctx_section.SkyLight) {
            std::cout << "missing SkyLight" << std::endl;
          }

          if (!ctx_section.BlockLight) {
            std::cout << "missing BlockLight" << std::endl;
          }

          if (!ctx_section.Blocks) {
            std::cout << "missing Blocks" << std::endl;
          }

          boost::shared_ptr<Section_Compound> s(new Legacy_Section_Compound(
            ctx_section.Y,
            ctx_section.Blocks,
            ctx_section.Data,
            ctx_section.SkyLight,
            ctx_section.BlockLight
          ));
          C->Level->Sections.push_back(s);
        } else {
          // Not only may a section be truncated but
          // their individual fields may as well.
          if (!ctx_section.BlockStates) {
            //std::cout << "missing BlockStates" << std::endl;
          }

          if (!ctx_section.SkyLight) {
            //std::cout << "missing SkyLight" << std::endl;
          }

          if (ctx_section.Palette.size() != ctx_section.PaletteProperties.size()) {
            std::cout << "bad palette sizes" << std::endl;
          }

          boost::shared_ptr<Section_Compound> s(new Modern_Section_Compound(
            ctx_section.Y,
            ctx_section.BlockStates,
            ctx_section.SkyLight,
            ctx_section.Palette,
            ctx_section.PaletteProperties
          ));
          C->Level->Sections.push_back(s);
        }
      }
    }
  }

  void begin_list(level_context* C, nbt::String name, nbt::Byte type, nbt::Int count) {
    if (name.compare("Sections") == 0) {
      C->p[C->pos++] = section_name::Sections;
      return;
    } else if(in_level_section(C)) {
      if (name.compare("Palette") == 0) {
        C->p[C->pos++] = section_name::Palette;
        return;
      }
    }

    C->p[C->pos++] = section_name::None;
  }

  void end_list(level_context* C, nbt::String name) {
    --C->pos;
  }

  void register_string(level_context* C, nbt::String name, nbt::String value) {
    if (in_palette_section(C)) {
      if (name.compare("Name") == 0) {
        C->sections.back().Palette.push_back(value);
      }
    } else if(in_palette_properties(C)) {
      std::pair<std::map<std::string, std::string>::iterator, bool> const& result =
        C->sections.back().PaletteProperties.back().insert(
          std::map<std::string, std::string>::value_type(name, value)
        );
      if (!result.second) {
        std::cout << "duplicated palette property: " << name << std::endl;
      }
    }
  }

  void register_byte(level_context* C, nbt::String name, nbt::Byte value) {
    if (in_level_section(C)) {
      if (name.compare("Y") == 0) {
        C->sections.back().Y = value;
      }
    }
  }

  void register_int(level_context* C, nbt::String name, nbt::Int i) {
    if (C->pos == 1 && C->p[0] == section_name::None) {
      if (name.compare("DataVersion") == 0) {
        C->Version = i;
      }
    }
  }

  void register_int_array(level_context* C, nbt::String name, nbt::IntArray* int_array) {
    delete int_array;
  }

  void register_byte_array(level_context* C, nbt::String name, nbt::ByteArray* byte_array) {
    if (in_level_section(C)) {
      if (name.compare("Data") == 0) {
        C->sections.back().Data.reset(byte_array);
        return;
      }

      if (name.compare("SkyLight") == 0) {
        C->sections.back().SkyLight.reset(byte_array);
        return;
      }

      if (name.compare("BlockLight") == 0) {
        C->sections.back().BlockLight.reset(byte_array);
        return;
      }

      if (name.compare("Blocks") == 0) {
        C->sections.back().Blocks.reset(byte_array);
        return;
      }
    }

    delete byte_array;
  }

  void register_long_array(level_context* C, nbt::String name, nbt::LongArray* long_array) {
    if (in_level_section(C)) {
      if (name.compare("BlockStates") == 0) {
        C->sections.back().BlockStates.reset(long_array);
        return;
      }
    }
    delete long_array;
  }

  void error_handler(level_context* C, size_t where, const char *why) {
    C->error = true;
    C->error_where = where;
    C->error_why = why;
  }

  level::~level(){
  }

  level::level(level_info_ptr _level_info) : _level_info(_level_info) {}

  std::string level::get_path() {
    return _level_info->get_path();
  }

  bool level::operator<(const level& other) const {
    return _level_info->get_coord() < other._level_info->get_coord();
  }

  time_t level::modification_time()
  {
    return _level_info->modification_time();
  }

  boost::shared_ptr<Level_Compound> level::get_level() {
    return Level;
  }

  /**
   * Get the bit slice (indice) for the given coordinates in the provided array.
   *
   * This method assumes truncated bit-packing.
   *
   * T here will typically be nbt::Byte or nbt::long
   * indice_bit_count is the number of bits in the slice and may not exceed 32 since the return type is int.
   */
  template<typename T, size_t indice_bit_count>
  inline boost::optional<int> get_indice(int x, int z, int y, boost::shared_ptr<nbt::Array<T>> &arr) {
    size_t indice_index = (y * 16 + z) * 16 + x;

    // Compiler should catch that this is static and pre-compute it,
    // specifically it will resolve statically since all arguments
    // are sourced from template parameters.
    size_t indice_count_in_element = static_cast<size_t>(floor(static_cast<double>(sizeof(T)*8) / static_cast<double>(indice_bit_count)));

    size_t index_in_element = indice_index % indice_count_in_element;
    size_t element_index = (indice_index - index_in_element) / indice_count_in_element;

    if (arr->length < 0 || element_index >= static_cast<size_t>(arr->length))
      return boost::optional<int>();

    T element = arr->values[element_index];
    int out_bits = ~(0xFFFFFFFF << indice_bit_count);
    return boost::optional<int>((element >> (indice_bit_count * index_in_element)) & out_bits);
  }

  /**
   * Get the bit slice (indice) for the given coordinates in the provided array.
   *
   * This method assumes optimal bit-packing.
   *
   * T here will typically be nbt::Byte or nbt::long
   * indice_bit_count is the number of bits in the slice and may not exceed 32 since the return type is int.
   */
  template<typename T, size_t indice_bit_count>
  inline boost::optional<int> get_stream_indice(int x, int z, int y, boost::shared_ptr<nbt::Array<T>> &arr) {
    size_t indice_index = (y * 16 + z) * 16 + x;

    size_t bits_into_stream = indice_index * indice_bit_count;
    size_t element_bits = sizeof(T) * 8;

    size_t bits_in_element = bits_into_stream % element_bits;
    size_t element_index = (bits_into_stream - bits_in_element) / element_bits;

    if (arr->length < 0 || element_index >= static_cast<size_t>(arr->length))
      return boost::optional<int>();

    T element = arr->values[element_index];
    int result = element >> bits_in_element;
    size_t got = element_bits - bits_in_element;
    if (got < indice_bit_count) {
      if (element_index + 1 >= static_cast<size_t>(arr->length))
        return boost::optional<int>();
      element = arr->values[element_index + 1];
      result = result | (element << got);
    }

    int out_bits = ~(0xFFFFFFFF << indice_bit_count);
    return boost::optional<int>(result & out_bits);
  }

  bool Modern_Section_Compound::get_block(BlockT& block, int x, int z, int y) {
    if (this->BlockStates) {
      boost::optional<int> block_data = boost::optional<int>();

      // Static expansion of dynamic palette resolver;
      // each section (16**3) contains 4096 unique blocks wihch yields max 2**12.
      if(this->stream_bit_packing) {
        // Special case bit-packing.
        if (this->palette_size > 16 && this->palette_size <= 32) {
          block_data = get_stream_indice<nbt::Long, 5>(x, z, y, this->BlockStates);
        } else if (this->palette_size > 32 && this->palette_size <= 64) {
          block_data = get_stream_indice<nbt::Long, 6>(x, z, y, this->BlockStates);
        } else if (this->palette_size > 64 && this->palette_size <= 128) {
          block_data = get_stream_indice<nbt::Long, 7>(x, z, y, this->BlockStates);
        } else if (this->palette_size > 256 && this->palette_size <= 512) {
          block_data = get_stream_indice<nbt::Long, 9>(x, z, y, this->BlockStates);
        } else if (this->palette_size > 512 && this->palette_size <= 1024) {
          block_data = get_stream_indice<nbt::Long, 10>(x, z, y, this->BlockStates);
        } else if (this->palette_size > 1024 && this->palette_size <= 2048) {
          block_data = get_stream_indice<nbt::Long, 11>(x, z, y, this->BlockStates);
        } else if (this->palette_size > 2048 && this->palette_size <= 4096) {
          block_data = get_stream_indice<nbt::Long, 12>(x, z, y, this->BlockStates);
        } else {
          // If this happens check the constructor logic when
          // stream_bit_packing decision is made.
          std::cout << "chunk segment uses unexpected bit-packing" << std::endl;
        }
      } else {
        // This is the common bit-packing method.
        if (this->palette_size <= 16) {
          block_data = get_indice<nbt::Long, 4>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 32) {
          block_data = get_indice<nbt::Long, 5>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 64) {
          block_data = get_indice<nbt::Long, 6>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 128) {
          block_data = get_indice<nbt::Long, 7>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 256) {
          block_data = get_indice<nbt::Long, 8>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 512) {
          block_data = get_indice<nbt::Long, 9>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 1024) {
          block_data = get_indice<nbt::Long, 10>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 2048) {
          block_data = get_indice<nbt::Long, 11>(x, z, y, this->BlockStates);
        } else if (this->palette_size <= 4096) {
          block_data = get_indice<nbt::Long, 12>(x, z, y, this->BlockStates);
        } else {
          // For this to happen the map format must change as each segment only can
          // contain a maximum of 4096 unique blocks.
          std::cout << "chunk segment has a larger palette then expected" << std::endl;
        }
      }

      if (block_data) {
        if (block_data.get() < this->palette_size && this->BlockPalette[block_data.get()]) {
          block = this->BlockPalette[block_data.get()].get();
          return true;
        }
      }
    }
    return false;
  }

  bool Legacy_Section_Compound::get_block(BlockT& block, int x, int z, int y) {
    boost::optional<int> lower_block_type;
    boost::optional<int> block_type = get_indice<nbt::Byte, 8>(x, z, y, this->Blocks);
    // Data values are packed two by two and the position LSB decides which
    // half-byte contains the requested block data value.
    boost::optional<int> block_data = get_indice<nbt::Byte, 4>(x, z, y, this->Data);

    if (block_type && block_data) {
      boost::optional<MaterialT*> material = get_material_legacy(block_type.get(), block_data.get());
      if (material) {
        block.properties.orientation = BlockOrientation::Invalid;
        block.properties.is_top = false;
        switch(material.get()->mode) {
        case MaterialMode::LargeFlowerBlock:
          block.properties.is_top = block_data.get() & 0x08;
          if(block.properties.is_top) {
            // y-coordinate must be positive, non-zero for top-blocks
            // since it is required to look at the bottom block as well,
            // Minecraft simply doesn't set the lower metadata bits for
            // the corresponding type on the top-block. As a technical
            // limitation this imply that bottom blocks of a chunk
            // cannot be large flower top-blocks, and those should be
            // bedrock anyway.
            if(y > 0) {
              lower_block_type = get_indice<nbt::Byte, 8>(x, z, y-1, this->Blocks);
              if (lower_block_type && lower_block_type.get() == block_type.get()) {
                block_data = get_indice<nbt::Byte, 4>(x, z, y-1, this->Data);
                if (block_data) {
                  material = get_material_legacy(block_type.get(), block_data.get() & 0x7);
                }
              }
            } else {
              // Top block not placed on a correct bottom block.
              // The expected LargeFlower multi block structure is invalid.
              material = boost::optional<MaterialT*>();
            }
          }
          break;
        case MaterialMode::LogBlock:
          switch((block_data.get() & 0xC) >> 2) {
            case 0:
              block.properties.orientation = BlockOrientation::UpDown;
              break;
            case 1:
              block.properties.orientation = BlockOrientation::EastWest;
              break;
            case 2:
              block.properties.orientation = BlockOrientation::NorthSouth;
              break;
            case 3:
              block.properties.orientation = BlockOrientation::OnlySides;
              break;
            default:
              block.properties.orientation = BlockOrientation::Invalid;
              break;
          }
          // Since metadata may result in the first sub-palette entry beign
          // returned, re-fetch with corrected metadata, however this will
          // only function correctly if all blocks in the same legacy
          // sub-palette has the same MaterialMode. Which is one of the
          // limitation of the legacy palette.
          material = get_material_legacy(block_type.get(), block_data.get() & 0x3);
          break;
        case MaterialMode::LegacyLeaves:
          // Since metadata may result in the first sub-palette entry beign
          // returned, re-fetch with corrected metadata, however this will
          // only function correctly if all blocks in the same legacy
          // sub-palette has the same MaterialMode. Which is one of the
          // limitation of the legacy palette.
          material = get_material_legacy(block_type.get(), block_data.get() & 0x3);
          break;
        default:
          // Normal lookup should be correct and no
          // special attributes should be needed.
          break;
        }
      }
      if (material) {
        block.material = material.get();
        return true;
      }
    }
    return false;
  }

  /**
   * might throw invalid_file if the file is not gramatically correct
   */
  void level::read(dynamic_buffer& buffer)
  {
    level_context context;

    nbt::Parser<level_context> parser(&context);

    parser.register_byte_array = register_byte_array;
    parser.register_int_array = register_int_array;
    parser.register_long_array = register_long_array;
    parser.register_byte = register_byte;
    parser.register_string = register_string;
    parser.register_int = register_int;
    parser.begin_compound = begin_compound;
    parser.begin_list = begin_list;
    parser.end_list = end_list;
    parser.end_compound = end_compound;
    parser.error_handler = error_handler;

    std::stringstream oss;

    uint32_t len;

    try {
      len = _level_info->get_region()->read_data(_level_info->get_x(),
          _level_info->get_z(), buffer);
    } catch(mc::bad_region& e) {
      throw invalid_file(e.what());
    }

    std::string chunk_data = oss.str();

    parser.parse_buffer(buffer.get(), len);

    if (context.error) {
      throw invalid_file(context.error_why);
    }

    if (!context.Level) {
      throw invalid_file("not a level data file");
    }

    Level = context.Level;
  }
}

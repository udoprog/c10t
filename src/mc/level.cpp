// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "mc/level.hpp"

#include "nbt/nbt.hpp"
#include "mc/region.hpp"
#include "mc/level_info.hpp"

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
    None
  };

  struct level_context {
    boost::shared_ptr<Level_Compound> Level;

    bool error;
    size_t error_where;
    const char* error_why;

    Section_Compound* tmp_Section;

    section_name p[64];
    int pos;

    level_context() : error(false), error_where(0), error_why("")
    {
      this->Level.reset(new Level_Compound);
      this->pos = 0;
    }
  };

  inline bool in_level_section(level_context* C) {
    return   C->pos == 4
          && C->p[0] == None
          && C->p[1] == Level
          && C->p[2] == Sections
          && C->p[3] == None;
  }

  void begin_compound(level_context* C, nbt::String name) {
    if (name.compare("Level") == 0) {
      C->p[C->pos++] = Level;
      return;
    }

    C->p[C->pos++] = None;

    if (in_level_section(C)) {
        C->tmp_Section = new Section_Compound;
        C->tmp_Section->Y = 0;
    }
  }

  void end_compound(level_context* C, nbt::String name) {
    if (in_level_section(C)) {
        if (!C->tmp_Section->Data) {
            std::cout << "missing Data" << std::endl;
        }

        if (!C->tmp_Section->SkyLight) {
            std::cout << "missing SkyLight" << std::endl;
        }

        if (!C->tmp_Section->BlockLight) {
            std::cout << "missing BlockLight" << std::endl;
        }

        if (!C->tmp_Section->Blocks) {
            std::cout << "missing Blocks" << std::endl;
        }

        C->Level->Sections.push_back(C->tmp_Section);
        C->tmp_Section = NULL;
    }

    --C->pos;
  }

  void begin_list(level_context* C, nbt::String name, nbt::Byte type, nbt::Int count) {
    if (name.compare("Sections") == 0) {
      C->p[C->pos++] = Sections;
      return;
    }

    C->p[C->pos++] = None;
  }

  void end_list(level_context* C, nbt::String name) {
    --C->pos;
  }

  void register_string(level_context* C, nbt::String name, nbt::String value) {
  }

  void register_byte(level_context* C, nbt::String name, nbt::Byte value) {
    if (in_level_section(C))
    {
      if (name.compare("Y") == 0) {
        C->tmp_Section->Y = value;
        return;
      }
    }
  }

  void register_int(level_context* C, nbt::String name, nbt::Int i) {
  }

  void register_int_array(level_context* C, nbt::String name, nbt::IntArray* int_array) {
    delete int_array;
  }

  void register_byte_array(level_context* C, nbt::String name, nbt::ByteArray* byte_array) {
    if (in_level_section(C))
    {
      if (name.compare("Data") == 0) {
        C->tmp_Section->Data.reset(byte_array);
        return;
      }

      if (name.compare("SkyLight") == 0) {
        C->tmp_Section->SkyLight.reset(byte_array);
        return;
      }

      if (name.compare("BlockLight") == 0) {
        C->tmp_Section->BlockLight.reset(byte_array);
        return;
      }

      if (name.compare("Blocks") == 0) {
        C->tmp_Section->Blocks.reset(byte_array);
        return;
      }
    }

    delete byte_array;
  }

  void register_long_array(level_context* C, nbt::String name, nbt::LongArray* long_array) {

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

  // T here will typically be nbt::Byte or nbt::long
  // indice_bit_count may not exceed 32 since the return type is int.
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

  bool Section_Compound::get_block(BlockT& block, int x, int z, int y) {
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

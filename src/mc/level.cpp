// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#include "mc/level.hpp"

#include "nbt/nbt.hpp"
#include "mc/region.hpp"
#include "mc/level_info.hpp"

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

/*
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
        ByteArray(BlockStates): (4096 bytes)
      }
    ]
  }
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

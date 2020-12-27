// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_LEVEL_HPP__
#define __MC_LEVEL_HPP__

#include <map>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/optional.hpp>

#include "mc/dynamic_buffer.hpp"
#include "mc/utils.hpp"
#include "mc/marker.hpp"

#include "nbt/types.hpp"
#include "mc/blocks.hpp"

namespace mc {
  namespace fs = boost::filesystem;

  class level_info;
  class level;
  class region;

  typedef boost::shared_ptr<level_info> level_info_ptr;
  typedef boost::shared_ptr<level> level_ptr;
  typedef boost::shared_ptr<region> region_ptr;

  class invalid_file : std::exception {
    private:
      const char* message;
    public:
      invalid_file(const char* message) : message(message) {}

      const char* what() const throw() {
        return message;
      }
  };

  class Section_Compound {
    private:
      nbt::Byte Y;
      boost::shared_ptr<nbt::ByteArray> SkyLight;

    protected:
      Section_Compound(nbt::Byte Y, boost::shared_ptr<nbt::ByteArray> SkyLight) : Y(Y), SkyLight(SkyLight) {};

    public:
      virtual bool get_block(BlockT &block, int x, int y, int z) = 0;
      nbt::Byte get_y() { return this->Y; }
  };

  class Modern_Section_Compound : public Section_Compound {
    private:
      size_t palette_size;
      boost::shared_ptr<nbt::LongArray> BlockStates;
      boost::shared_ptr<boost::optional<BlockT>[]> BlockPalette;

    public:
      Modern_Section_Compound(
        nbt::Byte Y,
        boost::shared_ptr<nbt::LongArray> BlockStates,
        boost::shared_ptr<nbt::ByteArray> SkyLight,
        std::vector<std::string> &Palette,
        std::vector<std::map<std::string, std::string>> &PaletteProperties
      );
      bool get_block(BlockT &block, int x, int y, int z);
  };

  class Legacy_Section_Compound : public Section_Compound {
    private:
      boost::shared_ptr<nbt::ByteArray> Blocks;
      boost::shared_ptr<nbt::ByteArray> Data;
      boost::shared_ptr<nbt::ByteArray> BlockLight;

    public:
      Legacy_Section_Compound(
        nbt::Byte Y,
        boost::shared_ptr<nbt::ByteArray> Blocks,
        boost::shared_ptr<nbt::ByteArray> Data,
        boost::shared_ptr<nbt::ByteArray> SkyLight,
        boost::shared_ptr<nbt::ByteArray> BlockLight
      ) : Section_Compound(Y, SkyLight), Blocks(Blocks), Data(Data), BlockLight(BlockLight) {};
      bool get_block(BlockT &block, int x, int y, int z);
  };

  struct Level_Compound {
    nbt::Int xPos;
    nbt::Int zPos;
    boost::shared_ptr<nbt::IntArray> HeightMap;
    std::vector<boost::shared_ptr<Section_Compound>> Sections;
  };

  class level
  {
    public:
      level(level_info_ptr _level_info);
      ~level();

      std::string get_path();
      time_t modification_time();

      /*
       * might throw invalid_file if the file is not gramatically correct
       */
      void read(dynamic_buffer& buffer);

      boost::shared_ptr<Level_Compound> get_level();

      bool operator<(const level& other) const;
    private:
      level_info_ptr _level_info;

      // these must be public for the parser to be able to reach them.
      std::vector<marker> signs;

      boost::shared_ptr<Level_Compound> Level;
  };
}

#endif /* __MC_LEVEL_HPP__ */

// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_LEVEL_HPP__
#define __MC_LEVEL_HPP__

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <nbt/nbt.hpp>
#include <mc/utils.hpp>
#include <mc/region.hpp>
#include <mc/world.hpp>

namespace mc {
  namespace fs = boost::filesystem;
  
  class invalid_file : std::exception {
    private:
      const char* message;
    public:
      invalid_file(const char* message) : message(message) {}

      const char* what() const throw() {
        return message;
      }
  };
  
  class level
  {
    public:
      typedef boost::shared_ptr<level_info> level_info_ptr;
    private:
      level_info_ptr _level_info;
      
      // these must be public for the parser to be able to reach them.
      std::vector<marker> signs;
      
      boost::shared_ptr<nbt::ByteArray> blocks;
      boost::shared_ptr<nbt::ByteArray> data;
      boost::shared_ptr<nbt::ByteArray> skylight;
      boost::shared_ptr<nbt::ByteArray> heightmap;
      boost::shared_ptr<nbt::ByteArray> blocklight;
    public:
      level(level_info_ptr _level_info);
      ~level();
      
      std::vector<marker> get_signs() {
        return signs;
      }

      std::string get_path() {
        return _level_info->get_path();
      }
      
      /*
       * might throw invalid_file if the file is not gramatically correct
       */
      void read(dynamic_buffer& buffer);
      
      boost::shared_ptr<nbt::ByteArray>
      get_blocks() {
        return blocks;
      }

      boost::shared_ptr<nbt::ByteArray>
      get_data() {
        return data;
      }

      boost::shared_ptr<nbt::ByteArray>
      get_skylight() {
        return skylight;
      }
      
      boost::shared_ptr<nbt::ByteArray>
      get_heightmap() {
        return heightmap;
      }
      
      boost::shared_ptr<nbt::ByteArray>
      get_blocklight() {
        return blocklight;
      }

      bool operator<(const level& other) const {
        return _level_info->get_coord() < other._level_info->get_coord();
      }
  };
}

#endif /* __MC_LEVEL_HPP__ */

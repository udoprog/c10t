// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef __MC_LEVEL_HPP__
#define __MC_LEVEL_HPP__

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include "mc/dynamic_buffer.hpp"
#include "mc/utils.hpp"
#include "mc/marker.hpp"

#include "nbt/types.hpp"

namespace mc {
  namespace fs = boost::filesystem;

  class level_info;
  class region;
  
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
      typedef boost::shared_ptr<region> region_ptr;
    public:
      level(level_info_ptr _level_info);
      ~level();
      
      std::vector<marker> get_signs();

      std::string get_path();
      
      /*
       * might throw invalid_file if the file is not gramatically correct
       */
      void read(dynamic_buffer& buffer);
      
      boost::shared_ptr<nbt::ByteArray>
      get_blocks();

      boost::shared_ptr<nbt::ByteArray>
      get_data();

      boost::shared_ptr<nbt::ByteArray>
      get_skylight();
      
      boost::shared_ptr<nbt::ByteArray>
      get_heightmap();
      
      boost::shared_ptr<nbt::ByteArray>
      get_blocklight();

      bool operator<(const level& other) const;
    private:
      level_info_ptr _level_info;
      
      // these must be public for the parser to be able to reach them.
      std::vector<marker> signs;
      
      boost::shared_ptr<nbt::ByteArray> blocks;
      boost::shared_ptr<nbt::ByteArray> data;
      boost::shared_ptr<nbt::ByteArray> skylight;
      boost::shared_ptr<nbt::ByteArray> heightmap;
      boost::shared_ptr<nbt::ByteArray> blocklight;
  };
}

#endif /* __MC_LEVEL_HPP__ */

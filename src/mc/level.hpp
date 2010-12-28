#ifndef __MC_LEVEL_HPP__
#define __MC_LEVEL_HPP__

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <nbt/nbt.hpp>
#include <mc/utils.hpp>

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
    private:
      bool complete;
      int x, z;
      const fs::path path;
      
      // these must be public for the parser to be able to reach them.
      std::vector<marker> signs;
      
      boost::shared_ptr<nbt::ByteArray> blocks;
      boost::shared_ptr<nbt::ByteArray> skylight;
      boost::shared_ptr<nbt::ByteArray> heightmap;
      boost::shared_ptr<nbt::ByteArray> blocklight;
    public:
      level(const fs::path path);
      ~level();
      
      std::vector<marker> get_signs() {
        return signs;
      }
      
      /*
       * might throw invalid_file if the file is not gramatically correct
       */
      void read();
      
      bool is_read() {
        return complete;
      }
      
      boost::shared_ptr<nbt::ByteArray>
      get_blocks() {
        return blocks;
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
  };
}

#endif /* __MC_LEVEL_HPP__ */

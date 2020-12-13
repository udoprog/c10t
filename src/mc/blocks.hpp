// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include <iostream>
#include <map>

#include <boost/optional.hpp>

#include "image/color.hpp"

namespace mc {
  enum MaterialMode {
    Block,
    HalfBlock,
    TorchBlock,
    LargeFlowerBlock,
    LogBlock,
    LegacySlab,
    LegacyLeaves
  };

  enum LegacyBlocks {
    Air = 0x00,
    Grass = 0x02,
    Leaves = 0x12
  };

  const char MATERIAL_MODE_BLOCK[] = "block";
  const char MATERIAL_MODE_HALF_BLOCK[] = "half_block";
  const char MATERIAL_MODE_TORCH_BLOCK[] = "torch_block";
  const char MATERIAL_MODE_LARGE_FLOWER_BLOCK[] = "large_plant_block";
  const char MATERIAL_MODE_LOG_BLOCK[] = "log_block";
  const char MATERIAL_MODE_LEGACY_SLAB[] = "legacy_slab_block";
  const char MATERIAL_MODE_LEGACY_LEAVES[] = "legacy_leaf_block";

  struct MaterialMode_tr_to_string {
    typedef MaterialMode internal_type;
    typedef std::string external_type;

    boost::optional<std::string> get_value(const MaterialMode &m) {
      switch(m) {
      case MaterialMode::Block:
        return boost::make_optional<std::string>(MATERIAL_MODE_BLOCK);
        break;
      case MaterialMode::HalfBlock:
        return boost::make_optional<std::string>(MATERIAL_MODE_HALF_BLOCK);
        break;
      case MaterialMode::TorchBlock:
        return boost::make_optional<std::string>(MATERIAL_MODE_TORCH_BLOCK);
        break;
      case MaterialMode::LargeFlowerBlock:
        return boost::make_optional<std::string>(MATERIAL_MODE_LARGE_FLOWER_BLOCK);
        break;
      case MaterialMode::LogBlock:
        return boost::make_optional<std::string>(MATERIAL_MODE_LOG_BLOCK);
        break;
      case MaterialMode::LegacySlab:
        return boost::make_optional<std::string>(MATERIAL_MODE_LEGACY_SLAB);
        break;
      case MaterialMode::LegacyLeaves:
        return boost::make_optional<std::string>(MATERIAL_MODE_LEGACY_LEAVES);
        break;
      default:
        return boost::optional<std::string>();
      }
    }
  };

  struct MaterialMode_tr_from_string {
    typedef std::string internal_type;
    typedef MaterialMode external_type;

    boost::optional<MaterialMode> get_value(const std::string &s) {
      if (MATERIAL_MODE_BLOCK == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::Block);
      } else if (MATERIAL_MODE_HALF_BLOCK == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::HalfBlock);
      } else if (MATERIAL_MODE_TORCH_BLOCK == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::TorchBlock);
      } else if (MATERIAL_MODE_LARGE_FLOWER_BLOCK == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::LargeFlowerBlock);
      } else if (MATERIAL_MODE_LOG_BLOCK == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::LogBlock);
      } else if (MATERIAL_MODE_LEGACY_SLAB == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::LegacySlab);
      } else if (MATERIAL_MODE_LEGACY_LEAVES == s) {
        return boost::make_optional<MaterialMode>(MaterialMode::LegacyLeaves);
      }
      return boost::optional<MaterialMode>();
    }
  };

  void reload_palette();
  void initialize_constants();
  void deinitialize_constants();

  extern const color SharedInvisColor;
  extern const color SharedDefaultColor;

  extern const int MapY;
  extern const int MapX;
  extern const int MapZ;

  typedef struct {
    std::string mc_namespace;
    std::string name;
    MaterialMode mode;
    color top;
    color side;
    std::vector<int> legacy_ids;
    int legacy_meta;
    bool enabled;
  } MaterialT;
  extern std::vector<MaterialT> MaterialTable;
  extern std::map<std::string, MaterialT*> MaterialMap;
  extern std::vector<std::vector<MaterialT*>> MaterialPaletteLegacy;

  inline boost::optional<MaterialT*> get_material_legacy(int material, int data) {
    // Legacy lookup needs to aggressivly fallback to the first metadata
    // variant when possible, this is because some blocks have either only
    // one variant with multiple states or even multiple
    // variants with multiple states.
    if (material >= 0 && static_cast<unsigned int>(material) < MaterialPaletteLegacy.size()) {
        if (data < 0) {
          data = 0;
        }
        unsigned int udata = static_cast<unsigned int>(data);
        std::vector<MaterialT*> subpallet = MaterialPaletteLegacy[material];
        if (subpallet.size() == 1) {
          udata = 0;
        }
        if (udata < subpallet.size() && subpallet[udata]) {
          return boost::optional<MaterialT*>(subpallet[udata]);
        } else if(subpallet.size() > 0 && subpallet[0]) {
          return subpallet[0];
        }
    }
    std::cout << "Unkown legacy material (" << std::hex << material << ":" << std::hex << data << ")" << std::endl;
    return boost::optional<MaterialT*>();
  }

  inline color get_color_legacy(int material, int data) {
    boost::optional<MaterialT*> blocktype = get_material_legacy(material, data);
    if (blocktype) {
      return blocktype.get()->top;
    } else {
      return SharedDefaultColor;
    }
  }

  inline color get_side_color_legacy(int material, int data) {
    boost::optional<MaterialT*> blocktype = get_material_legacy(material, data);
    if (blocktype) {
      return blocktype.get()->side;
    } else {
      return SharedDefaultColor;
    }
  }

  inline color get_color_legacy(int material) {
    return get_color_legacy(material, 0);
  }

  inline color get_side_color_legacy(int material) {
    return get_side_color_legacy(material, 0);
  }
}

#endif /* _BLOCKS_H_ */

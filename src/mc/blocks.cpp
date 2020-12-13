// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <iostream>
#include "blocks.hpp"

namespace mc {
  const color SharedInvisColor = color(0, 0, 0, 0);
  const color SharedDefaultColor = color(0, 0, 0, 0xff);

  const int MapX = 0x10;
  const int MapZ = 0x10;
  const int MapY = 0x100;

  std::vector<MaterialT> MaterialTable;
  std::map<std::string, MaterialT*> MaterialMap;
  std::vector<std::vector<MaterialT*>> MaterialPaletteLegacy;

  // Regenerate the internal lookup tables for materials (by name and by legacy id+meta).
  // The internal lookup tables use direct pointer into the primary material list;
  // therfore any changes to the size of that list must be directly followed by a call
  // to this method or undefined behaviour can be expected.
  void reload_palette() {
    MaterialMap.clear();
    MaterialPaletteLegacy.clear();
    MaterialTable.shrink_to_fit();
    MaterialPaletteLegacy.resize(MaterialTable.size());
    MaterialT *materials = MaterialTable.data();
    for (size_t i = 0; i < MaterialTable.size(); i++) {
      MaterialT *material = &materials[i];
      std::string fq_material = material->mc_namespace + ":" + material->name;
      std::pair<std::map<std::string, MaterialT*>::iterator, bool> const& result =
        MaterialMap.insert(std::map<std::string, MaterialT*>::value_type(fq_material, material));
      if (!result.second) {
        std::cerr << "Palette loader: Duplicated name " << fq_material << std::endl;
        // For now the last parsed material shall take precedence, this is similar
        // to the legacy loader below.
        result.first->second = material;
      }
      BOOST_FOREACH(int j, material->legacy_ids) {
        if (material->legacy_meta >= 0 && material->legacy_meta < 16) {
            if (MaterialPaletteLegacy[j].size() == 0 && material->legacy_meta == 0) {
              MaterialPaletteLegacy[j].resize(1);
            } else if (MaterialPaletteLegacy[j].size() <= static_cast<size_t>(material->legacy_meta)) {
              MaterialPaletteLegacy[j].resize(16);
            }
            if (MaterialPaletteLegacy[j][material->legacy_meta]) {
              MaterialT *previous = MaterialPaletteLegacy[j][material->legacy_meta];
              std::cerr << "Legacy Palette loader: " << fq_material << " is overwriting " << previous->mc_namespace << ":" << previous->name << std::endl;
            }
            MaterialPaletteLegacy[j][material->legacy_meta] = material;
        } else {
          // This code should not be reached; unless a programming error is present or memory corruption occurred.
          // JSON loader code shall refuse to load blocks that would result in this state.
          std::cerr << "Legacy Palette loader: Invalid internal state for " << fq_material << ", legacy lookup cannot be initailized." << std::endl;
        }
      }
    }
  }

  void initialize_constants() {
  }

  void deinitialize_constants() {
    MaterialMap.clear();
    MaterialPaletteLegacy.clear();
  }
}

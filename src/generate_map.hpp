#ifndef __GENERATE_MAP_HPP__
#define __GENERATE_MAP_HPP__

#include <ostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

#include "settings_t.hpp"

bool generate_map(
    std::ostream& out,
    std::ostream& out_log,
    std::ostream& error,
    settings_t &s,
    std::vector<std::string>& hints,
    boost::filesystem::path& world_path,
    boost::filesystem::path& output_path);

#endif /*__GENERATE_MAP_HPP__*/

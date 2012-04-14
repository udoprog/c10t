#ifndef __GENERATE_STATISTICS_HPP__
#define __GENERATE_STATISTICS_HPP__

#include <ostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

#include "settings_t.hpp"

bool generate_statistics(
    std::ostream& out,
    std::ostream& out_log,
    std::ostream& error,
    settings_t &s,
    std::vector<std::string>& hints,
    boost::filesystem::path& world_path,
    boost::filesystem::path& output_path);

#endif /*__GENERATE_STATISTICS_HPP__*/

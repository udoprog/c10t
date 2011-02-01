#include "warps.hpp"

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

void warps_db::read(std::vector<warp>& warps)
{
  using ::boost::lexical_cast;
  using ::boost::split;
  
  if (!fs::is_regular_file(path)) {
    throw warps_db_exception("database does not exist");
  }
  
  ifstream fp(path.string().c_str());
  
  if (!fp.good())
  {
    throw warps_db_exception("could not open file");
  }
  
  while (!fp.eof())
  {
    string line;
    getline(fp, line);
    
    if (line.empty()) continue;
    
    vector<string> parts;
    split(parts, line, boost::is_any_of(":"));
    
    if (parts.size() < 4) continue;
    
    warp w;
    
    w.name = parts[0];
    
    try {
      w.xPos = int(lexical_cast<double>(parts[1]));
      w.yPos = int(lexical_cast<double>(parts[2]));
      w.zPos = int(lexical_cast<double>(parts[3]));
    } catch(...) {
      // silently ignore
      continue;
    }
    
    warps.push_back(w);
  }
  
  fp.close();
}

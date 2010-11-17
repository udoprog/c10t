#include "warps.hpp"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
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
      w.xPos = lexical_cast<int>(parts[1]);
      w.yPos = lexical_cast<int>(parts[2]);
      w.zPos = lexical_cast<int>(parts[3]);
    } catch(...) {
      // silently ignore
      continue;
    }
    
    warps.push_back(w);
  }
  
  fp.close();
}

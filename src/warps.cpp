#include "warps.h"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
using namespace std;

warps_db::warps_db(settings_t& s)
{
  if (!s.show_warps)
    return;
  
  if (!fs::is_regular_file(s.show_warps_path)) {
    cerr << "Can't find warps file: " << s.show_warps_path.string() << endl;
    return;
  }
  
  ifstream fp;
  fp.open(s.show_warps_path.string().c_str());
  if (!fp.good())
  {
    cerr << "Error opening " << s.show_warps_path.string() << endl;
    return;
  }
  
  while (!fp.eof())
  {
    string str;
    getline(fp, str);
    if (str.empty()) continue;
    
    vector<string> bits;
    boost::split(bits, str, boost::is_any_of(":"));
    if (bits.size() < 4) continue;
    
    warp w;
    
    w.name = bits[0];
    char *p;
    w.xPos = int(strtod(bits[1].c_str(), &p));
    while (p && *p != '\0' && isspace(*p)) p++;
    if (!p || *p != '\0') { cerr << "Bad number: " << bits[1] << endl; continue; }
    w.yPos = int(strtod(bits[2].c_str(), &p));
    while (p && *p != '\0' && isspace(*p)) p++;
    if (!p || *p != '\0') { cerr << "Bad number: " << bits[2] << endl; continue; }
    w.zPos = int(strtod(bits[3].c_str(), &p));
    while (p && *p != '\0' && isspace(*p)) p++;
    if (!p || *p != '\0') { cerr << "Bad number: " << bits[3] << endl; continue; }
    
    warps.push_back(w);
  }
  fp.close();
}
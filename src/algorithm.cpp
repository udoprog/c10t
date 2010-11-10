#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace nonstd {
  // hackish split
  void split(std::vector<std::string>& v, const std::string& str, char delim)
  {
    std::stringstream ss(str);
    std::string item;
    
    while(std::getline(ss, item, delim)) {
      v.push_back(item);
    }
  }
}

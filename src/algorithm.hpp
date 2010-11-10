#ifndef NONSTD_ALGORITHM
#define NONSTD_ALGORITHM

namespace nonstd {
  /**
   * The fastest portable split I could find, only limitation is that it splits on only one character, but that's O.K.
   **/
  void split(std::vector<std::string>& v, const std::string& str, char delim);
}

#endif /* NONSTD_ALGORITHM */

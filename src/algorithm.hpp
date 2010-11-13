#ifndef NONSTD_ALGORITHM
#define NONSTD_ALGORITHM

#include <stdlib.h>

namespace nonstd {
  /**
   * The fastest portable split I could find, only limitation is that it splits on only one character, but that's O.K.
   **/
  void split(std::vector<std::string>& v, const std::string& str, char delim);
  
  template<typename T>
  class reporting {
    public:
      virtual void add(T) = 0;
      virtual void done(T) = 0;
      virtual void set_limit(T) = 0;
  };
  
  template<typename T>
  class continious : public reporting<T> {
    protected:
      const static uintmax_t LINE_WIDTH = 30;
    private:
      T chunks;
      T total;
      T limit;
      
      unsigned int line;
      
      typedef void (*progress)(T);
      typedef void (*endline)(T);
      
      progress progress_f;
      endline endline_f;
    public:
      continious(T limit, progress progress_f, endline endline_f) :
        chunks(0), total(0), limit(limit), line(0), 
        progress_f(progress_f), endline_f(endline_f)
      {}
      
      virtual void add(T parts) {
        chunks += parts;
        
        while (chunks > limit) {
          chunks -= limit;
          total += limit;
          
          progress_f(total);
          
          if (!(line++ < LINE_WIDTH)) {
            endline_f(total);
            line = 0;
          }
        }
      }

      virtual void set_limit(T limit) {
      }
      
      void done(T last_part) {
        total += last_part;
        endline_f(total);
      }
  };
  
  template<typename T>
  class limited : public reporting<T> {
    protected:
      const static uintmax_t LINE_WIDTH = 30;
    private:
      T chunks;
      T total;
      T limit;
      
      unsigned int line;
      
      typedef void (*progress)(T);
      typedef void (*endline)(T, T);
      
      progress progress_f;
      endline endline_f;

      T total_limit;
      bool total_limit_set;
    public:
      limited(T limit, progress progress_f, endline endline_f) :
        chunks(0), total(0), limit(limit), line(0), 
        progress_f(progress_f), endline_f(endline_f), total_limit(0), total_limit_set(false)
      {}
      
      virtual void add(T parts) {
        chunks += parts;
        
        while (chunks > limit) {
          chunks -= limit;
          total += limit;
          
          progress_f(total);
          
          if (line++ >= LINE_WIDTH) {
            endline_f(total, total_limit);
            line = 0;
          }
        }
      }
      
      virtual void set_limit(T limit) {
        total_limit = limit;
        total_limit_set = true;
      }
      
      void done(T last) {
        total += last;
        endline_f(total_limit, total_limit);
      }
  };
}

#endif /* NONSTD_ALGORITHM */

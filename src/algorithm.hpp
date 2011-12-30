#ifndef NONSTD_ALGORITHM
#define NONSTD_ALGORITHM

#include <stdlib.h>
#include <ostream>

namespace nonstd {
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
    public:
      typedef void (*progress_func)(std::ostream&, T);
      typedef void (*endline_func)(std::ostream&, T);

      continious(std::ostream& out, T limit, progress_func progress_f, endline_func endline_f) :
        out(out), chunks(0), total(0), limit(limit), line(0), 
        progress_f(progress_f), endline_f(endline_f)
      {}
      
      virtual void add(T parts) {
        chunks += parts;
        
        while (chunks > limit) {
          chunks -= limit;
          total += limit;
          
          progress_f(out, total);
          
          if (!(line++ < LINE_WIDTH)) {
            endline_f(out, total);
            line = 0;
          }
        }
      }

      virtual void set_limit(T limit) {
      }
      
      void done(T last_part) {
        total += chunks + last_part;
        endline_f(out, total);
      }
    private:
      std::ostream& out;
      T chunks;
      T total;
      T limit;
      
      unsigned int line;
      
      progress_func progress_f;
      endline_func endline_f;
  };
  
  template<typename T>
  class limited : public reporting<T> {
    protected:
      const static uintmax_t LINE_WIDTH = 30;
    public:
      typedef void (*progress_func)(std::ostream&, T);
      typedef void (*endline_func)(std::ostream&, T, T);

      limited(std::ostream& out, T limit, progress_func progress_f, endline_func endline_f) :
        out(out), chunks(0), total(0), limit(limit), line(0), 
        progress_f(progress_f), endline_f(endline_f), total_limit(0), total_limit_set(false)
      {}
      
      virtual void add(T parts) {
        chunks += parts;
        
        while (chunks > limit) {
          chunks -= limit;
          total += limit;
          
          progress_f(out, total);
          
          if (line++ >= LINE_WIDTH) {
            endline_f(out, total, total_limit);
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
        endline_f(out, total_limit, total_limit);
      }
    private:
      std::ostream& out;
      T chunks;
      T total;
      T limit;
      
      unsigned int line;
      
      progress_func progress_f;
      endline_func endline_f;

      T total_limit;
      bool total_limit_set;
  };
}

#endif /* NONSTD_ALGORITHM */

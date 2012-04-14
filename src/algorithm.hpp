#ifndef NONSTD_ALGORITHM
#define NONSTD_ALGORITHM

#include <stdlib.h>
#include <ostream>

namespace nonstd
{
  template<typename T>
  class reporting
  {
    public:
      virtual void add(T) = 0;
      virtual void done(T) = 0;
  };

  template<typename T, typename O = std::ostream>
  class continious : public reporting<T>
  {
    public:
      const static uintmax_t LINE_WIDTH = 30;

      typedef void (*progress_func)(O&, T);
      typedef void (*endline_func)(O&, T);

      continious(O& out,
                 T progress_threshold,
                 progress_func progress_f,
                 endline_func endline_f)
      : out(out),
        chunks(0),
        total(0),
        progress_threshold(progress_threshold),
        line(0), 
        progress_f(progress_f),
        endline_f(endline_f)
      {
      }
      
      virtual void add(T parts)
      {
        chunks += parts;
        
        while (chunks > progress_threshold)
        {
          chunks -= progress_threshold;
          total += progress_threshold;
          
          progress_f(out, total);
          
          if (!(line++ < LINE_WIDTH)) {
            endline_f(out, total);
            line = 0;
          }
        }
      }
      
      void done(T last_part)
      {
        total += chunks + last_part;
        endline_f(out, total);
      }
    private:
      O& out;
      T chunks;
      T total;
      T progress_threshold;
      
      unsigned int line;
      
      progress_func progress_f;
      endline_func endline_f;
  };
  
  template<typename T, typename O = std::ostream>
  class limited : public reporting<T>
  {
    public:
      const static uintmax_t LINE_WIDTH = 30;

      typedef void (*progress_func)(O&, T);
      typedef void (*endline_func)(O&, T, T);

      limited(O& out,
              T progress_threshold,
              T total_limit,
              progress_func progress_f,
              endline_func endline_f)
      : out(out),
        chunks(0),
        total(0),
        progress_threshold(progress_threshold),
        line(0), 
        progress_f(progress_f),
        endline_f(endline_f),
        total_limit(total_limit)
      {
      }
      
      virtual void add(T parts)
      {
        chunks += parts;
        
        while (chunks > progress_threshold)
        {
          chunks -= progress_threshold;
          total += progress_threshold;
          
          progress_f(out, total);
          
          if (line++ >= LINE_WIDTH)
          {
            endline_f(out, total, total_limit);
            line = 0;
          }
        }
      }
      
      void done(T last)
      {
        total += last;
        endline_f(out, total_limit, total_limit);
      }
    private:
      O& out;

      T chunks;
      T total;
      T progress_threshold;
      
      unsigned int line;
      
      progress_func progress_f;
      endline_func endline_f;

      T total_limit;
  };
}

#endif /* NONSTD_ALGORITHM */

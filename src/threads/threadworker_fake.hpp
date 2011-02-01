#ifndef __THREADWORKER_SUB_HPP__
#define __THREADWORKER_SUB_HPP__

template <class I, class O>
class threadworker
{
private:
  std::queue<I> in;
  
  const int thread_count;
public:
  threadworker(int c) : thread_count(c) {
  }
  
  virtual ~threadworker() {
  }
  
  void give(I t) {
    in.push(t);
  }
  
  void start() {
  }
  
  void run(int id) {
  }

  virtual O work(I) = 0;
  
  O get() {
    I i = in.front();
    in.pop();
    return work(i);
  }
  
  void join() {
  }
};

#endif /*__THREADWORKER_SUB_HPP__*/

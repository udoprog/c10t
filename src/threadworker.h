#ifndef _THREADWORKER_H_
#define _THREADWORKER_H_
#include <queue>
#include <list>

#include <boost/detail/atomic_count.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

template <class I, class O>
class threadworker
{
private:
  std::queue<I> in;
  std::queue<O> out;
  std::list<boost::thread *> threads;
  boost::condition in_cond;
  boost::condition out_cond;
  boost::mutex in_mutex;
  boost::mutex out_mutex;
  volatile int running;
public:
  threadworker(int c) : running(1) {
    for (int i = 0; i < c; i++) {
      boost::thread *t = new boost::thread(boost::bind(&threadworker::run, this, i));
      threads.push_back(t);
    }
  }

  ~threadworker() {
    for (std::list<boost::thread *>::iterator it = threads.begin(); it != threads.end(); it++)
    {
      delete *it;
    }
  }
  
  void give(I t) {
    boost::mutex::scoped_lock lock(in_mutex);
    in.push(t);
    in_cond.notify_one();
  }
  
  void run(int id) {
    while (running) {
      I i;
      
      {
        boost::mutex::scoped_lock lock(in_mutex);
        
        while (in.empty()) {
          in_cond.wait(lock);
          
          if (!running) {
            return;
          }
        }
        
        i = in.front();
        in.pop();
      }
      
      O o = work(i);
      
      {
        boost::mutex::scoped_lock lock(out_mutex);
        out.push(o);
        out_cond.notify_one();
      }
    }
  }

  virtual O work(I) = 0;
  
  O get() {
    O o;
    
    boost::mutex::scoped_lock lock(out_mutex);
    
    while (out.empty()) {
      out_cond.wait(lock);
    }
    
    o = out.front();
    out.pop();
    
    return o;
  }

  void join() {
    running = 0;
    
    {
      boost::mutex::scoped_lock lock(in_mutex);
      in_cond.notify_all();
    }
    
    for (std::list<boost::thread *>::iterator it = threads.begin(); it != threads.end(); it++)
    {
      (*it)->join();
    }
  }
};
#endif /* _THREADWORKER_H_ */

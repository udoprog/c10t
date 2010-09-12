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
  std::queue<I> queue_in;
  std::queue<O> queue_out;
  std::list<boost::thread *> threads;
  boost::mutex queue_in_mutex;
  boost::mutex queue_out_mutex;
  boost::condition queue_in_cond;
  boost::condition queue_out_cond;
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
    boost::mutex::scoped_lock lock(queue_in_mutex);
    queue_in.push(t);
    queue_in_cond.notify_one();
  }
  
  void run(int id) {
    while (running) {
      I i;
      
      {
        boost::mutex::scoped_lock lock(queue_in_mutex);
        
        while (queue_in.empty()) {
          queue_in_cond.wait(lock);
          
          if (!running) {
            return;
          }
        }
        
        i = queue_in.front();
        queue_in.pop();
      }
      
      O o = work(i);
      
      {
        boost::mutex::scoped_lock lock(queue_out_mutex);
        queue_out.push(o);
        queue_out_cond.notify_one();
      }
    }
  }

  virtual O work(I) = 0;
  
  O yield() {
    O o;
    
    boost::mutex::scoped_lock lock(queue_out_mutex);
    
    while (queue_out.empty()) {
      queue_out_cond.wait(lock);
    }
    
    o = queue_out.front();
    queue_out.pop();
    
    return o;
  }

  void join() {
    running = 0;
    
    {
      boost::mutex::scoped_lock lock(queue_in_mutex);
      queue_in_cond.notify_all();
    }
    
    for (std::list<boost::thread *>::iterator it = threads.begin(); it != threads.end(); it++)
    {
      (*it)->join();
    }
  }
};
#endif /* _THREADWORKER_H_ */

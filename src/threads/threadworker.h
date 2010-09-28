// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _THREADWORKER_H_
#define _THREADWORKER_H_
#include <assert.h>

#include <queue>
#include <list>

#include <boost/detail/atomic_count.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <iostream>

template <class I, class O>
class threadworker
{
private:
  std::queue<I> in;
  std::queue<O> out;
  std::list<boost::thread *> threads;
  boost::condition in_cond;
  boost::condition out_cond;
  boost::condition start_cond;
  boost::mutex in_mutex;
  boost::mutex out_mutex;
  boost::mutex start_mutex;
  
  const int thread_count;
  volatile int running;
  volatile int started;
  volatile int input;
  boost::detail::atomic_count output;
public:
  threadworker(int c) : thread_count(c), running(1), started(0), input(0), output(1) {
    for (int i = 0; i < c; i++) {
      boost::thread *t = new boost::thread(boost::bind(&threadworker::run, this, i));
      threads.push_back(t);
    }
  }
  
  virtual ~threadworker() {
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
  
  void start() {
    started = 1;
    boost::mutex::scoped_lock lock(start_mutex);
    start_cond.notify_all();
  }
  
  void run(int id) {
    {
      boost::mutex::scoped_lock lock(start_mutex);
      
      while (!started) {
        start_cond.wait(lock);
      }
    }
    
    while (running) {
      I i;

      int qp;
      
      {
        boost::mutex::scoped_lock lock(in_mutex);
        
        while (running && in.empty()) {
          in_cond.wait(lock);
        }
        
        if (!running) {
          return;
        }
        
        i = in.front();
        in.pop();
        qp = ++input;
      }
      
      O o = work(i);
      
      {
        // first, make sure that we output the results in the same order they came in
        // try to make it as lock-free as possible
        while (qp != output) {
          boost::thread::yield();
        }
        
        boost::mutex::scoped_lock lock(out_mutex);
        out.push(o);
        out_cond.notify_one();
        ++output;
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

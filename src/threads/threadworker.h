// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _THREADWORKER_H_
#define _THREADWORKER_H_
#include <assert.h>

#include <queue>
#include <list>

#include <iostream>

#if !defined(C10T_DISABLE_THREADS)
#include <boost/detail/atomic_count.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

template<typename T>
class sync_queue {
  private:
    std::queue<T> q;
    boost::mutex mutex;
    boost::condition empty_cond;
  public:
    void add(T o) {
      boost::mutex::scoped_lock lock(mutex);
      q.push(o);
      empty_cond.notify_one();
    }
    
    T take() {
      boost::mutex::scoped_lock lock(mutex);
      while (q.empty()) {
        empty_cond.wait(lock);
      }

      T o = q.front();
      q.pop();
      return o;
    }
    
    bool empty() {
      boost::mutex::scoped_lock lock(mutex);
      return q.empty();
    }
};

template <class I, class O>
class threadworker
{
private:
  int total;

  sync_queue<I> in;
  sync_queue<O> out;
  boost::condition start_cond;
  boost::condition order_cond;
  boost::mutex start_mutex;
  boost::mutex order_mutex;
  
  const int thread_count;
  
  volatile int running;
  volatile int started;
  boost::detail::atomic_count input;
  boost::detail::atomic_count output;
  
  std::list<boost::thread*> threads;
public:
  threadworker(int c, int total) : total(total), thread_count(c), running(1), started(0), input(0), output(1) {
    for (int i = 0; i < c; i++) {
      boost::thread* t = new boost::thread(boost::bind(&threadworker::run, this, i));
      threads.push_back(t);
    }
  }
  
  virtual ~threadworker() {
    for (std::list<boost::thread*>::iterator it = threads.begin(); it != threads.end(); it++)
    {
      delete *it;
    }
  }
  
  void give(I t) {
    in.add(t);
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

    int qp;
    
    while ((qp = ++input) <= total) {
      I i = in.take();
      
      O o = work(i);
      
      {
        // first, make sure that we output the results in the same order they came in
        // try to make it as lock-free as possible
        boost::mutex::scoped_lock lock(order_mutex);
        
        while (qp != output) {
          order_cond.wait(lock);
        }
        
        ++output;
        order_cond.notify_all();
      }
      
      out.add(o);
    }
  }

  virtual O work(I) = 0;
  
  O get() {
    return out.take();
  }
  
  void join() {
    running = 0;
    
    for (std::list<boost::thread*>::iterator it = threads.begin(); it != threads.end(); it++)
    {
      (*it)->join();
    }
  }
};
#else
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

#endif

#endif /* _THREADWORKER_H_ */

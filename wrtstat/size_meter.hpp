#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>

namespace wrtstat {

struct size_meter
{
  typedef size_meter self;
  typedef std::shared_ptr<self> self_ptr;

  typedef types::set_span_fun_t set_span_fun_t;
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  typedef types::size_type size_type;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  size_meter(time_type now, size_type count, size_type multiple, set_span_fun_t fun, mutex_ptr pmutex)
    : now(now)
    , count(count)
    , multiple(multiple)
    , timer_fun(fun)
    , wmutex(pmutex)
  {
  }

  ~size_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    
    if ( auto pmutex = wmutex.lock() ) 
    {
      std::lock_guard<mutex_type> lk(*pmutex);
      timer_fun( now, count*multiple, count );
    }
  };

  self_ptr clone(time_type now, size_type count) const
  {
    if ( auto pmutex = wmutex.lock() )
      return std::make_shared<self>(now, count, multiple, timer_fun, pmutex);
    return nullptr;
  }

  void reset() 
  {
    now = 0;
    timer_fun = 0;
  }

  
  time_type now;
  size_type count;
  size_type multiple;
  set_span_fun_t timer_fun;
  mutex_wptr wmutex;
};

}

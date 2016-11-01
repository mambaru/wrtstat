#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>

namespace wrtstat {

template<typename D>
struct time_meter
{
  typedef std::chrono::high_resolution_clock clock_type;
  typedef D duration_type;
  typedef types::set_span_fun_t set_span_fun_t;
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  time_meter(time_type now, set_span_fun_t fun, mutex_ptr pmutex)
    : now(now)
    , timer_fun(fun)
    , wmutex(pmutex)
  {
    start = clock_type::now();
  }

  ~time_meter()
  {
    if (timer_fun==nullptr)
      return;
    if ( auto pmutex = wmutex.lock() ) 
    {
      clock_type::time_point finish = clock_type::now();
      span_type span = std::chrono::template duration_cast<D>( finish - start ).count();
      std::lock_guard<mutex_type> lk(*pmutex);
      timer_fun( now, span );
    }
  };

  time_type now;
  set_span_fun_t timer_fun;
  mutex_wptr wmutex;
  clock_type::time_point start;
};

}

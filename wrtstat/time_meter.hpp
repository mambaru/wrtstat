#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>

namespace wrtstat {

template<typename D>
struct time_meter
{
  typedef time_meter<D> self;
  typedef std::shared_ptr<self> self_ptr;

  typedef std::chrono::high_resolution_clock clock_type;
  typedef D duration_type;
  typedef types::set_span_fun_t set_span_fun_t;
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  typedef types::size_type size_type;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  time_meter(time_type now, size_type count, set_span_fun_t fun, mutex_ptr pmutex)
    : now(now)
    , count(count)
    , timer_fun(fun)
    , wmutex(pmutex)
  {
    start = clock_type::now();
  }

  ~time_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    if ( auto pmutex = wmutex.lock() ) 
    {
      clock_type::time_point finish = clock_type::now();
      span_type span = std::chrono::template duration_cast<D>( finish - start ).count();
      std::lock_guard<mutex_type> lk(*pmutex);
      timer_fun( now, span, count );
    }
  };

  self_ptr clone(time_type now, size_type count) const
  {
    if ( auto pmutex = wmutex.lock() )
      return std::make_shared<self>(now, count, timer_fun, pmutex );
    return nullptr;
  }
  
  time_type now;
  size_type count;
  set_span_fun_t timer_fun;
  mutex_wptr wmutex;
  clock_type::time_point start;
};

}

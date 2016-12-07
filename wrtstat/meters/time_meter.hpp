#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

template<typename D>
struct time_meter
{
  typedef time_meter<D> self;
  typedef std::shared_ptr<self> self_ptr;

  // typedef std::chrono::high_resolution_clock clock_type;
  typedef std::chrono::steady_clock clock_type;
  typedef D duration_type;
  typedef types::meter_fun_t meter_fun_t;
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  typedef types::size_type size_type;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  time_meter( const time_meter& ) = delete;
  time_meter& operator=( const time_meter& ) = delete;

  time_meter(time_type now, size_type count, meter_fun_t fun)
    : now(now)
    , count(count)
    , meter_fun(fun)
  {
    start = clock_type::now();
  }

  ~time_meter()
  {
    if ( meter_fun == nullptr || now == 0 )
      return;
    clock_type::time_point finish = clock_type::now();
    span_type span = std::chrono::template duration_cast<D>( finish - start ).count();
    meter_fun( now, span, count );
  };
  
  void reset() 
  {
    now = 0;
    count = 0;
  }

  self_ptr clone(time_type now, size_type count) const
  {
    return std::make_shared<self>(now, count, meter_fun );
  }

  time_type now;
  size_type count;
  meter_fun_t meter_fun;
  clock_type::time_point start;
};

}

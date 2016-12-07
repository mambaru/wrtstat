#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

struct size_meter
{
  typedef size_meter self;
  typedef std::shared_ptr<self> self_ptr;

  typedef types::meter_fun_t meter_fun_t;
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  typedef types::size_type size_type;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  size_meter( const size_meter& ) = delete;
  size_meter& operator=( const size_meter& ) = delete;


  size_meter(time_type now, size_type size, size_type count, meter_fun_t fun/*, mutex_ptr pmutex*/)
    : now(now)
    , size(size)
    , count(count)
    , timer_fun(fun)
  {
  }

  ~size_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    timer_fun( now, size, count );
  };

  self_ptr clone(time_type now, size_type size,  size_type count) const
  {
    return std::make_shared<self>(now, size, count, timer_fun);
  }

  void reset() 
  {
    now = 0;
    count = 0;
  }

  void inc(size_type size, size_type count) 
  {
    this->size+=size;
    this->count+=count;
  }

  
  time_type now;
  size_type size;
  size_type count;
  meter_fun_t timer_fun;
  mutex_wptr wmutex;
};

}

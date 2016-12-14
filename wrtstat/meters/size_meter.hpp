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


  size_meter(meter_fun_t fun, time_type now, size_type size)
    : now(now)
    , size(size)
    , timer_fun(fun)
  {
  }

  ~size_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    timer_fun( now, size, size );
  };

  void set_size(size_t size) { this->size = size; }
  size_type get_size() const { return this->size; }

  self_ptr clone(time_type now, size_type size) const
  {
    return std::make_shared<self>(timer_fun, now, size);
  }

  void reset() 
  {
    now = 0;
  }

  time_type now;
  size_type size;
  meter_fun_t timer_fun;
};

}

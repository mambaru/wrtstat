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

  
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  typedef types::size_type size_type;
  typedef std::function< void(time_type now, time_type value, size_type count) > meter_fun_t;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  size_meter( const size_meter& ) = delete;
  size_meter& operator=( const size_meter& ) = delete;


  size_meter(meter_fun_t fun, time_type ts_now, size_type s)
    : now(ts_now)
    , size(s)
    , timer_fun(fun)
  {
  }

  ~size_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    timer_fun( now, static_cast<span_type>(size), size );
  };

  void set_size(size_t s) { this->size = s; }
  size_type get_size() const { return this->size; }

  self_ptr clone(time_type ts_now, size_type s) const
  {
    return std::make_shared<self>(timer_fun, ts_now, s);
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

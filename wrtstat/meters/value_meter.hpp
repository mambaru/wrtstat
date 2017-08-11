#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

struct value_meter
{
  typedef value_meter self;
  typedef std::shared_ptr<self> self_ptr;

  typedef types::meter_fun_t meter_fun_t;
  typedef types::time_type time_type;
  typedef types::span_type span_type;
  typedef types::size_type size_type;
  
  typedef types::mutex_type mutex_type;
  typedef types::mutex_ptr mutex_ptr;
  typedef types::mutex_wptr mutex_wptr;

  value_meter( const size_meter& ) = delete;
  value_meter& operator=( const value_meter& ) = delete;


  value_meter(meter_fun_t fun, time_type ts_now, size_type val, size_type cnt = 1)
    : now(ts_now)
    , value(val)
    , count(cnt)
    , timer_fun(fun)
  {
  }

  ~value_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    timer_fun( now, static_cast<span_type>(value), count );
  };

  void set_value(size_t val) { this->value = val; }
  size_type get_value() const { return this->value; }

  void set_count(size_t cnt) { this->count = cnt; }
  size_type get_count() const { return this->count; }

  self_ptr clone(time_type ts_now, size_type val,  size_type cnt) const
  {
    return std::make_shared<self>(timer_fun, ts_now, val, cnt);
  }

  void reset() 
  {
    now = 0;
    count = 0;
  }

  time_type now;
  size_type value;
  size_type count;
  meter_fun_t timer_fun;
  mutex_wptr wmutex;
};

}

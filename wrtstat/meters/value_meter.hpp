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


  value_meter(meter_fun_t fun, time_type now, size_type value, size_type count = 1)
    : now(now)
    , value(value)
    , count(count)
    , timer_fun(fun)
  {
  }

  ~value_meter()
  {
    if ( timer_fun == nullptr || now == 0)
      return;
    timer_fun( now, value, count );
  };

  void set_value(size_t value) { this->value = value; }
  size_type get_value() const { return this->value; }

  void set_count(size_t count) { this->count = count; }
  size_type get_count() const { return this->count; }

  self_ptr clone(time_type now, size_type value,  size_type count) const
  {
    return std::make_shared<self>(timer_fun, now, value, count);
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

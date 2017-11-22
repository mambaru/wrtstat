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

  typedef std::chrono::steady_clock clock_type;
  typedef D duration_type;
  
  typedef std::function< void(time_type now, value_type value, size_type count) > meter_fun_t;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::weak_ptr<mutex_type> mutex_wptr;

  time_meter( const time_meter& ) = delete;
  time_meter& operator=( const time_meter& ) = delete;

  time_meter(const meter_fun_t& fun, time_type ts_now, size_type cnt = 1 )
    : now(ts_now)
    , count(cnt)
    , meter_fun(fun)
  {
    start = clock_type::now();
  }

  ~time_meter()
  {
    if ( meter_fun == nullptr || now == 0 )
      return;
    clock_type::time_point finish = clock_type::now();
    time_type span = std::chrono::template duration_cast<D>( finish - start ).count();
    meter_fun( now, static_cast<value_type>(span), count );
  };
  
  void reset() 
  {
    now = 0;
    count = 0;
  }

  self_ptr clone(time_type ts_now, size_type cnt) const
  {
    return std::make_shared<self>(meter_fun, ts_now, cnt);
  }

  time_type now;
  size_type count;
  meter_fun_t meter_fun;
  clock_type::time_point start;
};

}

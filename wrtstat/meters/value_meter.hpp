#pragma once

#include <wrtstat/types.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

class value_meter
{
public:
  typedef value_meter self;
  typedef std::shared_ptr<self> self_ptr;

  typedef std::function< void(time_type now, value_type value, size_type count) > meter_fun_t;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::weak_ptr<mutex_type> mutex_wptr;

  value_meter( const size_meter& ) = delete;
  value_meter& operator=( const value_meter& ) = delete;


  value_meter(const meter_fun_t& fun, time_type ts_now, value_type val, size_type cnt = 1)
    : _now(ts_now)
    , _value(val)
    , _count(cnt)
    , _meter_fun(fun)
  {
  }

  ~value_meter()
  {
    if ( _meter_fun == nullptr || _now == 0)
      return;
    _meter_fun( _now, static_cast<value_type>(_value), _count );
  };

  void set_value(value_type val) { _value = val; }
  value_type get_value() const { return _value; }

  void set_count(size_t cnt) { _count = cnt; }
  size_type get_count() const { return _count; }

  self_ptr clone(time_type ts_now, size_type val,  size_type cnt) const
  {
    return std::make_shared<self>(_meter_fun, ts_now, val, cnt);
  }

  void reset() 
  {
    _now = 0;
    _count = 0;
    _value = 0;
  }
private:
  time_type  _now;
  value_type _value;
  size_type  _count;
  meter_fun_t _meter_fun;
};

class value_meter_factory: value_meter
{
  typedef value_meter super;
public:
  typedef typename super::meter_fun_t meter_fun_t;
  
  value_meter_factory( const meter_fun_t& fun, time_type resolution)
    : super(fun, 0, 0, 0)
    , _resolution(resolution)
  {}
  
  std::shared_ptr< value_meter > create(size_type value, size_type count) const
  {
    time_type now_ts = aggregator::now(_resolution);
    return super::clone(now_ts, value, count);
  }
  
private:
  time_type _resolution;
};

}

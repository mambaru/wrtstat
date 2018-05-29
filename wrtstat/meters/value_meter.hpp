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

  value_meter() = delete;
  value_meter( value_meter&& ) = default;
  value_meter& operator=( value_meter&& ) = default;
  
  value_meter( const value_meter& ) = delete;
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
    this->push_();
  };

  void push_()
  {
    if ( _meter_fun == nullptr || _now == 0)
      return;
    _meter_fun( _now, static_cast<value_type>(_value), _count );
  };

  void set_value(value_type val) { _value = val; }
  value_type get_value() const { return _value; }

  void set_count(size_t cnt) { _count = cnt; }
  size_type get_count() const { return _count; }

  value_meter clone(time_type ts_now, value_type val,  size_type cnt) const
  {
    return value_meter(_meter_fun, ts_now, val, cnt);
  }

  void reset() 
  {
    _now = 0;
    _count = 0;
    _value = 0;
  }
  
  void reset(const meter_fun_t& fun, time_type ts_now, value_type val,  size_type cnt )
  {
    this->push_();
    _now = ts_now;
    _meter_fun = fun;
    _value = val;
    _count = cnt;
  }
private:
  time_type  _now;
  value_type _value;
  size_type  _count;
  meter_fun_t _meter_fun;
};

class value_meter_factory//: value_meter
{
  //typedef value_meter super;
public:
  typedef value_meter meter_type;
  typedef value_meter::meter_fun_t meter_fun_t;
  
  value_meter_factory( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {}

  value_meter create(value_type value, size_type count) const
  {
    return this->create(aggregator::now(_resolution), value, count);
  }

  value_meter create(time_type now_ts,  value_type value, size_type count) const
  {
    return value_meter(_meter_fun, now_ts, value, count);
  }

  std::shared_ptr< value_meter > create_shared(value_type value, size_type count) const
  {
    return this->create_shared(aggregator::now(_resolution), value, count);
  }

  std::shared_ptr< value_meter > create_shared(time_type now_ts,  value_type value, size_type count) const
  {
    return std::make_shared<value_meter>(_meter_fun, now_ts, value, count);
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

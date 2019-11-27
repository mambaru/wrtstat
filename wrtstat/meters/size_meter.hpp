#pragma once

#include <wrtstat/aggregator.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

class size_point
{
public:
  typedef size_point self;
  typedef std::shared_ptr<self> self_ptr;
  typedef std::function< void(time_type now, value_type value, size_type count) > meter_fun_t;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::weak_ptr<mutex_type> mutex_wptr;

  size_point()= delete;
  size_point( size_point&& ) = default;
  size_point& operator=( size_point&& ) = default;
  size_point( const size_point& ) = delete;
  size_point& operator=( const size_point& ) = delete;

  size_point(const meter_fun_t& fun, time_type ts_now, value_type s)
    : _now(ts_now)
    , _size(s)
    , _meter_fun(fun)
  {
  }

  ~size_point()
  {
    this->_push();
  }

  void _push()
  {
    if ( _meter_fun == nullptr || _now == 0)
      return;
    _meter_fun( _now, _size, static_cast<size_type>(_size) );
  }

  void set_size(value_type s) { _size = s; }
  value_type get_size() const { return _size; }

  size_point clone(time_type ts_now, value_type s) const
  {
    return self(_meter_fun, ts_now, s);
  }

  void reset()
  {
    _now = 0;
    _size = 0;
  }

  void reset(const meter_fun_t& fun, time_type ts_now, value_type size )
  {
    this->_push();
    _now = ts_now;
    _meter_fun = fun;
    _size = size;
  }

private:
  time_type _now;
  value_type _size;
  meter_fun_t _meter_fun;
};

class size_meter
{
public:
  typedef size_point point_type;
  typedef size_point::meter_fun_t meter_fun_t;

  size_meter( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {}

  size_point create(value_type size) const
  {
    return this->create(aggregator::now(_resolution), size);
  }

  size_point create(time_type now_ts, value_type size) const
  {
    return size_point(_meter_fun, now_ts, size);
  }


  std::shared_ptr< size_point > create_shared(value_type size) const
  {
    return this->create_shared(aggregator::now(_resolution), size);
  }

  std::shared_ptr< size_point > create_shared(time_type now_ts, value_type size) const
  {
    return std::make_shared<size_point>(_meter_fun, now_ts, size);
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

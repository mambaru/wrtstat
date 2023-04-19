#pragma once

#include <wrtstat/aggregator/api/types.hpp>
#include <memory>
#include <functional> 

namespace wrtstat {

class value_point
{
public:
  typedef value_point self;
  typedef std::shared_ptr<self> self_ptr;

  typedef std::function< void(time_type now, value_type value, size_type count) > meter_fun_t;

  value_point() = delete;
  value_point( value_point&& ) = default;
  value_point& operator=( value_point&& ) = default;

  value_point( const value_point& ) = delete;
  value_point& operator=( const value_point& ) = delete;

  value_point(const meter_fun_t& fun, resolutions res, value_type val, size_type cnt = 1)
    : _resolution(res)
    , _value(val)
    , _count(cnt)
    , _meter_fun(fun)
  {
  }

  ~value_point()
  {
    this->push();
  }

  void push(time_type ts_now)
  {
    if ( _meter_fun == nullptr )
      return;
    _meter_fun( ts_now, _value, _count );
  }

  void push()
  {
    this->push(aggregator::now(_resolution));
  }

  void set_value(value_type val) { _value = val; }
  value_type get_value() const { return _value; }

  void set_count(size_t cnt) { _count = cnt; }
  size_type get_count() const { return _count; }

  value_point clone( value_type val,  size_type cnt) const
  {
    return value_point(_meter_fun, _resolution, val, cnt);
  }

  void reset()
  {
    _count = 0;
    _value = 0;
  }

  void reset(const meter_fun_t& fun,  value_type val,  size_type cnt )
  {
    this->push();
    _meter_fun = fun;
    _value = val;
    _count = cnt;
  }
private:
  resolutions _resolution;
  value_type _value;
  size_type  _count;
  meter_fun_t _meter_fun;
};

class value_meter
{
public:
  typedef value_point point_type;
  typedef value_point::meter_fun_t meter_fun_t;

  value_meter( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {}

  value_point create(value_type value, size_type count) const
  {
    return value_point(_meter_fun, _resolution, value, count);
  }

  std::shared_ptr< value_point > create_shared(value_type value, size_type count) const
  {
    return std::make_shared<value_point>(_meter_fun, _resolution, value, count);
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

#pragma once

#include <wrtstat/aggregator/aggregator.hpp>
#include <chrono>
#include <memory>

namespace wrtstat {

class size_point
{
public:
  typedef size_point self;
  typedef std::shared_ptr<self> self_ptr;
  typedef std::function< void(time_type now, value_type value, size_type count) > meter_fun_t;

  size_point()= delete;
  size_point( size_point&& ) = default;
  size_point& operator=( size_point&& ) = default;
  size_point( const size_point& ) = delete;
  size_point& operator=( const size_point& ) = delete;

  size_point(const meter_fun_t& fun, resolutions res, value_type s)
    : _resolution(res)
    , _size(s)
    , _meter_fun(fun)
  {
  }

  ~size_point()
  {
    this->push();
  }

  void push(time_type now_ts)
  {
    if ( _meter_fun == nullptr )
      return;
    _meter_fun( now_ts, _size, static_cast<size_type>(_size) );
    _meter_fun=nullptr;
  }

  void push()
  {
    this->push(aggregator::now(_resolution));
  }

  void set_size(value_type s) { _size = s; }
  value_type get_size() const { return _size; }

  size_point clone( value_type s) const
  {
    return self(_meter_fun, _resolution, s);
  }

  void reset()
  {
    _size = 0;
  }

  void reset(const meter_fun_t& fun, value_type size )
  {
    this->push();
    _meter_fun = fun;
    _size = size;
  }

private:
  resolutions _resolution;
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

  size_point create( value_type size) const
  {
    return size_point(_meter_fun, _resolution, size);
  }

  std::shared_ptr< size_point > create_shared(value_type size) const
  {
    return std::make_shared<size_point>(_meter_fun, _resolution, size);
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

#pragma once

#include <wrtstat/aggregator.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

class size_meter
{
public:
  typedef size_meter self;
  typedef std::shared_ptr<self> self_ptr;
  typedef std::function< void(time_type now, value_type value, size_type count) > meter_fun_t;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::weak_ptr<mutex_type> mutex_wptr;

  size_meter()= delete;
  size_meter( size_meter&& ) = default;
  size_meter& operator=( size_meter&& ) = default;
  size_meter( const size_meter& ) = delete;
  size_meter& operator=( const size_meter& ) = delete;

  

  size_meter(const meter_fun_t& fun, time_type ts_now, size_type s)
    : _now(ts_now)
    , _size(s)
    , _meter_fun(fun)
  {
  }

  ~size_meter()
  {
    this->_push();
  };
  
  void _push()
  {
    if ( _meter_fun == nullptr || _now == 0)
      return;
    _meter_fun( _now, static_cast<value_type>(_size), _size );
  };

  void set_size(size_t s) { _size = s; }
  size_type get_size() const { return _size; }

  size_meter clone(time_type ts_now, size_type s) const
  {
    return self(_meter_fun, ts_now, s);
  }

  void reset() 
  {
    _now = 0;
    _size = 0;
  }
  
  void reset(meter_fun_t fun, time_type ts_now, size_type size )
  {
    this->_push();
    _now = ts_now;
    _meter_fun = fun;
    _size = size;
  }


private:
  time_type _now;
  size_type _size;
  meter_fun_t _meter_fun;
};


class size_meter_factory/*: size_meter*/
{
  //typedef size_meter super;
public:
  typedef size_meter meter_type;
  typedef size_meter::meter_fun_t meter_fun_t;
  
  size_meter_factory( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {
  
  }

  size_meter create(size_type size) const
  {
    return this->create(aggregator::now(_resolution), size);
  }

  size_meter create(time_type now_ts, size_type size) const
  {
    return size_meter(_meter_fun, now_ts, size);
  }

  
  std::shared_ptr< size_meter > create_shared(size_type size) const
  {
    return this->create_shared(aggregator::now(_resolution), size);
  }

  std::shared_ptr< size_meter > create_shared(time_type now_ts, size_type size) const
  {
    return std::make_shared<size_meter>(_meter_fun, now_ts, size);
    //return _pool->make(_meter_fun, now_ts, size);
    //return super::clone(now_ts, size);
    /*auto p = _pool->make(_meter_fun, now_ts, size);
    if (p==nullptr) abort();
    return p;*/
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

#pragma once

#include <wrtstat/types.hpp>
#include <wrtstat/aggregator.hpp>
#include <chrono>
#include <memory>
#include <iostream>

namespace wrtstat {

template<typename D>
class time_meter
{
public:
  typedef time_meter<D> self;
  typedef std::shared_ptr<self> self_ptr;

  typedef std::chrono::steady_clock clock_type;
  typedef D duration_type;
  
  typedef std::function< void(time_type now, value_type span, size_type count) > meter_fun_t;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::weak_ptr<mutex_type> mutex_wptr;

  time_meter()= delete;
  time_meter( const time_meter& ) = delete;
  time_meter& operator=( const time_meter& ) = delete;
  time_meter( time_meter&& ) = default;
  time_meter& operator=( time_meter&& ) = default;
  
  time_meter(const meter_fun_t& fun, time_type ts_now, size_type cnt = 1 )
    : _now(ts_now)
    , _count(cnt)
    , _meter_fun(fun)
  {
    if ( _now != 0 )
      _start = clock_type::now();
  }

  ~time_meter()
  {
    this->_push();
  };
  
  void reset() 
  {
    _now = 0;
    _count = 0;
  }

  void reset(const meter_fun_t& fun, time_type ts_now, size_type cnt )
  {
    this->_push();
    _now = ts_now;
    _meter_fun = fun;
    if ( _now != 0 )
    {
      _count = cnt;
      _start = clock_type::now();
    }
  }
  
  void _push()
  {
    if ( _meter_fun == nullptr || _now == 0 )
      return;
    clock_type::time_point finish = clock_type::now();
    time_type span = std::chrono::template duration_cast<D>( finish - _start ).count();
    _meter_fun( _now, static_cast<value_type>(span), _count );
  }

  time_meter<D> clone(time_type ts_now, size_type cnt) const
  {
    return time_meter<D>(_meter_fun, ts_now, cnt);
  }

private:
  time_type _now;
  size_type _count;
  meter_fun_t _meter_fun;
  clock_type::time_point _start;
};

template<typename D>
class time_meter_factory
{
public:
  typedef time_meter<D> meter_type;
  typedef typename time_meter<D>::meter_fun_t meter_fun_t;
  
  time_meter_factory( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {
  }

  time_meter<D> create(size_type count) const
  {
    return this->create(aggregator::now(_resolution), count);
  }

  time_meter<D> create(time_t ts_now, size_type count) const
  {
    return time_meter<D>(_meter_fun, ts_now, count);
  }

  std::shared_ptr< time_meter<D> > create_shared(size_type count) const
  {
    return this->create_shared(aggregator::now(_resolution), count);
  }

  std::shared_ptr< time_meter<D> > create_shared(time_t ts_now, size_type count) const
  {
    return std::make_shared<time_meter<D> >(_meter_fun, ts_now, count);
  }
 
private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

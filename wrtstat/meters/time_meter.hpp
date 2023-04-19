#pragma once

#include <wrtstat/aggregator/aggregator.hpp>
#include <fas/typemanip.hpp>
#include <fas/utility/useless_cast.hpp>

#include <chrono>
#include <memory>

namespace wrtstat {

template<typename D>
class time_point
{
public:
  typedef time_point<D> self;
  typedef std::shared_ptr<self> self_ptr;

  typedef std::chrono::steady_clock clock_type;
  typedef D duration_type;

  typedef std::function< void(time_type now, value_type span, size_type count) > meter_fun_t;

  time_point()= delete;
  time_point( const time_point& ) = delete;
  time_point& operator=( const time_point& ) = delete;
  time_point( time_point&& ) = default;
  time_point& operator=( time_point&& ) = default;

  time_point(const meter_fun_t& fun, resolutions res, size_type cnt = 1 )
    : _resolution(res)
    , _count(cnt)
    , _meter_fun(fun)
  {
    _start = clock_type::now();
  }

  ~time_point()
  {
    this->push();
  }

  void reset()
  {
    _count = 0;
  }

  void reset(const meter_fun_t& fun, size_type cnt )
  {
    this->push();
    _meter_fun = fun;
    _count = cnt;
    _start = clock_type::now();
  }

  void push(time_type ts_now)
  {
    if ( _meter_fun == nullptr )
      return;
    clock_type::time_point finish = clock_type::now();
    time_type span = std::chrono::template duration_cast<D>( finish - _start ).count();
    _meter_fun( ts_now, fas::useless_cast<value_type>(span), _count );
    _meter_fun=nullptr;
  }

  void push()
  {
    this->push(aggregator::now(_resolution));
  }

  time_point<D> clone( size_type cnt) const
  {
    return time_point<D>(_meter_fun, _resolution, cnt);
  }

private:
  resolutions _resolution;
  size_type _count;
  meter_fun_t _meter_fun;
  clock_type::time_point _start;
};

template<typename D>
class time_meter
{
public:
  typedef time_point<D> point_type;
  typedef typename time_point<D>::meter_fun_t meter_fun_t;

  time_meter( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {
  }

  time_point<D> create(size_type count) const
  {
    return time_point<D>(_meter_fun, _resolution, count);
  }

  std::shared_ptr< time_point<D> > create_shared(size_type count) const
  {
    return std::make_shared<time_point<D> >(_meter_fun, _resolution, count);
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

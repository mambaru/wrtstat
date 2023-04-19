#pragma once

#include <string>
#include <wrtstat/aggregator/aggregator.hpp>
#include <fas/utility/useless_cast.hpp>
#include <memory>


namespace wrtstat {

template<typename D>
class composite_point
{
public:
  typedef composite_point<D> self;
  typedef std::shared_ptr<composite_point> self_ptr;

  typedef std::chrono::steady_clock clock_type;
  typedef D duration_type;

  typedef std::function< void(time_type now, value_type span, size_type count,
                              value_type readed, value_type writed) > meter_fun_t;

  composite_point()= delete;
  composite_point( const composite_point& ) = delete;
  composite_point& operator=( const composite_point& ) = delete;
  composite_point( composite_point&& ) = default;
  composite_point& operator=( composite_point&& ) = default;

  composite_point(const meter_fun_t& fun, resolutions res, size_type count, value_type readed, value_type writed, time_type shift_back = 0)
    : _resolution(res)
    , _count(count)
    , _readed(readed)
    , _writed(writed)
    , _meter_fun(fun)
  {
    _start = clock_type::now();
    if ( shift_back != 0 )
      _start -= D(shift_back);
  }

  ~composite_point()
  {
    this->push();
  }

  void reset()
  {
    _count = 0;
    _readed = 0;
    _writed = 0;
  }

  void reset(const meter_fun_t& fun, size_type count = 1, value_type readed = 0, value_type writed = 0,
             time_type shift_back = 0 )
  {
    this->push();
    _meter_fun = fun;
    _count = count;
    _readed = readed;
    _writed = writed;
    _start = clock_type::now();
    if ( shift_back != 0 )
      _start -= D(shift_back);
  }
  
  void set_start_time(clock_type::time_point start_time)
  {
    _start = start_time;
  }

  value_type get_read_size() const
  {
    return _readed;
  }

  void set_read_size(value_type size)
  {
    _readed = size;
  }

  value_type get_write_size() const
  {
    return _writed;
  }

  void set_write_size(value_type size)
  {
    _writed = size;
  }

  void push(time_type now_ts)
  {
    if ( _meter_fun == nullptr )
      return;
    
    clock_type::time_point finish = clock_type::now();
    time_type span = std::chrono::template duration_cast<D>( finish - _start ).count();
    _meter_fun( now_ts, fas::useless_cast<value_type>(span), _count, _readed, _writed );
    _meter_fun=nullptr;
  }

  void push()
  {
    this->push(aggregator::now(_resolution));
  }

  composite_point<D> clone( size_type count, value_type readed = 0, value_type writed = 0,
                           time_type shift_back = 0) const
  {
    return composite_point<D>(_meter_fun, _resolution, count, readed, writed, shift_back);
  }

private:
  resolutions _resolution;
  size_type _count;
  value_type _readed;
  value_type _writed;
  meter_fun_t _meter_fun;
  clock_type::time_point _start;
};

template<typename D>
class composite_meter
{
public:
  typedef composite_point<D> point_type;
  typedef typename composite_point<D>::meter_fun_t meter_fun_t;

  composite_meter( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {
  }

  composite_point<D> create(size_type count, value_type readed, value_type writed, time_type shift_back = 0) const
  {
    return composite_point<D>(_meter_fun, _resolution, count, readed, writed, shift_back);

  }

  std::shared_ptr< composite_point<D> > create_shared(size_type count,
                                                      value_type readed, value_type writed, time_type shift_back = 0) const
  {
    return std::make_shared<composite_point<D> >(_meter_fun, _resolution, count, readed, writed, shift_back);
  }

private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

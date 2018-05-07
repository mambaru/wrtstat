#pragma once

#include <string>
#include <wrtstat/aggregator.hpp>
#include <memory>


namespace wrtstat {

template<typename D>
class composite_meter
{
public:
  typedef composite_meter<D> self;
  typedef std::shared_ptr<composite_meter> self_ptr;

  typedef std::chrono::steady_clock clock_type;
  typedef D duration_type;
  
  typedef std::function< void(time_type now, value_type span, size_type count, 
                              size_type readed, size_type writed) > meter_fun_t;
                              
  composite_meter()= delete; 
  composite_meter( const composite_meter& ) = delete;
  composite_meter& operator=( const composite_meter& ) = delete;
  composite_meter( composite_meter&& ) = default;
  composite_meter& operator=( composite_meter&& ) = default;

  composite_meter(const meter_fun_t& fun, time_type ts_now, size_type count, size_type readed, size_type writed)
    : _now(ts_now)
    , _count(count)
    , _readed(readed)
    , _writed(writed)
    , _meter_fun(fun)
  {
    if ( _now != 0 )
      _start = clock_type::now();
  }

  ~composite_meter()
  {
    this->_push();
  };
  
  void reset() 
  {
    _now = 0;
    _count = 0;
    _readed = 0;
    _writed = 0;
  }

  void reset(meter_fun_t fun, time_type ts_now, size_type count = 1, size_type readed = 0, size_type writed = 0 )
  {
    this->_push();
    _now = ts_now;
    _meter_fun = fun;
    if ( _now != 0 )
    {
      _count = count;
      _readed = readed;
      _writed = writed;
      _start = clock_type::now();
    }
  }
  
  size_type get_read_size() const 
  {
    return _readed;
  }
  
  void set_read_size(size_type size) 
  {
    _readed = size;
  }

  size_type get_write_size() const 
  {
    return _writed;
  }

  void set_write_size(size_type size) 
  {
    _writed = size;
  }
  
  void _push()
  {
    if ( _meter_fun == nullptr || _now == 0 )
      return;
    clock_type::time_point finish = clock_type::now();
    time_type span = std::chrono::template duration_cast<D>( finish - _start ).count();
    _meter_fun( _now, static_cast<value_type>(span), _count, _readed, _writed );
  }

  composite_meter<D> clone(time_type ts_now, size_type count, size_type readed = 0, size_type writed = 0) const
  {
    return composite_meter<D>(_meter_fun, ts_now, count, readed, writed);
  }

private:
  time_type _now;
  size_type _count;
  size_type _readed;
  size_type _writed;
  meter_fun_t _meter_fun;
  clock_type::time_point _start;
};

template<typename D>
class composite_meter_factory
{
public:
  typedef composite_meter<D> meter_type;
  typedef typename composite_meter<D>::meter_fun_t meter_fun_t;
  
  composite_meter_factory( const meter_fun_t& fun, resolutions resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {
  }

  composite_meter<D> create(size_type count, size_type readed, size_type writed) const
  {
    return this->create(aggregator::now(_resolution), count, readed, writed);
  }

  composite_meter<D> create(time_t ts_now, size_type count, size_type readed, size_type writed) const
  {
    return composite_meter<D>(_meter_fun, ts_now, count, readed, writed);
    
  }

  std::shared_ptr< composite_meter<D> > create_shared(size_type count, size_type readed, size_type writed) const
  {
    return this->create_shared(aggregator::now(_resolution), count, readed, writed);
  }

  std::shared_ptr< composite_meter<D> > create_shared(time_t ts_now, size_type count, size_type readed, size_type writed) const
  {
    return std::make_shared<composite_meter<D> >(_meter_fun, ts_now, count, readed, writed);
  }
 
private:
  meter_fun_t _meter_fun;
  resolutions _resolution;
};

}

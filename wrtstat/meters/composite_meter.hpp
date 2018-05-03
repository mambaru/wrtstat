#pragma once

#include <string>
#include <memory>
#include <wrtstat/meters/factory_pool.hpp>

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
                              
  
  composite_meter( const composite_meter& ) = delete;
  composite_meter& operator=( const composite_meter& ) = delete;

  composite_meter()= default;  
  
  composite_meter(const meter_fun_t& fun, time_type ts_now, size_type count = 1, 
                  size_type readed = 0, size_type writed = 0)
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

  self_ptr clone(time_type ts_now, size_type count, size_type readed = 0, size_type writed = 0) const
  {
    return std::make_shared<self>(_meter_fun, ts_now, count, readed, writed);
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
  typedef typename composite_meter<D>::meter_fun_t meter_fun_t;
  typedef factory_pool< composite_meter<D> > pool_type;
  typedef std::shared_ptr<pool_type> pool_ptr;
  
  composite_meter_factory( const meter_fun_t& fun, time_type resolution)
    : _meter_fun(fun)
    , _resolution(resolution)
  {
    _pool = std::make_shared<pool_type>(0);
  }
  
  std::shared_ptr< composite_meter<D> > create(size_type count, size_type readed = 0, size_type writed = 0) const
  {
    return this->create(aggregator::now(_resolution), count, readed, writed);
  }

  std::shared_ptr< composite_meter<D> > create(time_t ts_now, size_type count, size_type readed = 0, size_type writed = 0) const
  {
    //return std::make_shared<time_meter<D>>(_meter_fun, ts_now, count);
    return _pool->make(_meter_fun, ts_now, count, readed, writed);
  }
 
private:
  meter_fun_t _meter_fun;
  time_type _resolution;
  pool_ptr _pool;
};

  /*
template<typename D>
class composite_meter
{
public:
  typedef composite_meter<D> self_type;
  typedef std::shared_ptr<self_type> self_ptr;
  typedef D duration_type;
  typedef time_meter< duration_type > time_meter_type;
  typedef size_meter size_meter_type;
  typedef std::shared_ptr<time_meter_type> time_meter_ptr;
  typedef std::shared_ptr<size_meter_type> size_meter_ptr;

  composite_meter(const time_meter_ptr& tm, const size_meter_ptr& rm, const size_meter_ptr& wm)
    : _time_meter(tm)
    , _read_meter(rm)
    , _write_meter(wm)
  {
  }
  
  void reset()
  {
    if ( _time_meter!=nullptr )
      _time_meter->reset();
    if ( _read_meter!=nullptr )
      _read_meter->reset();
    if ( _write_meter!=nullptr )
      _write_meter->reset();
  }

  void reset()
  {
    if ( _time_meter!=nullptr )
      _time_meter->reset();
    if ( _read_meter!=nullptr )
      _read_meter->reset();
    if ( _write_meter!=nullptr )
      _write_meter->reset();
  }

  size_type get_read_size() const 
  {
    if ( _read_meter!=nullptr )
      return _read_meter->get_size();
    return 0;
  }
  
  void set_read_size(size_type size) 
  {
    if ( _read_meter!=nullptr )
      _read_meter->set_size(size);
  }

  size_type get_write_size() const 
  {
    if ( _write_meter!=nullptr )
      return _write_meter->get_size();
    return 0;
  }

  void set_write_size(size_type size) 
  {
    if ( _write_meter!=nullptr )
      _write_meter->set_size(size);
  }
  
  self_ptr clone(time_type now, size_t size) const
  {
    time_meter_ptr time_meter;
    size_meter_ptr read_meter;
    size_meter_ptr write_meter;
    if ( _time_meter!=nullptr )
      time_meter = _time_meter->clone(now, 1);
    if ( _read_meter!=nullptr )
      read_meter = _read_meter->clone(now, size);
    if ( _write_meter!=nullptr )
      write_meter = _write_meter->clone(now, 0);

    return std::make_shared<composite_meter>(time_meter, read_meter, write_meter);
  }

private:
  time_meter_ptr _time_meter;
  size_meter_ptr _read_meter;
  size_meter_ptr _write_meter;
};

template<typename D>
class composite_meter_factory
{
  typedef composite_meter<D> super;
public:
  typedef std::shared_ptr< time_meter_factory<D> > time_factory_ptr;
  typedef std::shared_ptr< size_meter_factory > size_factory_ptr;
  typedef factory_pool< composite_meter<D> > pool_type;
  typedef std::shared_ptr<pool_type> pool_ptr;

  composite_meter_factory( 
    const time_factory_ptr& tm, 
    const size_factory_ptr& rm, 
    const size_factory_ptr& wm, 
    time_type resolution
  ) : _tm(tm), _rm(rm), _wm(wm)
    , _resolution(resolution)
  {
    _pool = std::make_shared<pool_type>(0);
  }
  
  std::shared_ptr< composite_meter<D> > create(size_type size) const
  {
    return this->create(aggregator::now(_resolution), size);
  }

  std::shared_ptr< composite_meter<D> > create(time_type now_ts, size_type size) const
  {
    return _pool->make(      
      _tm->create(now_ts, 1),
      _rm->create(now_ts, size),
      _wm->create(now_ts, 0)
    );
  }

private:
  time_factory_ptr _tm;
  size_factory_ptr _rm;
  size_factory_ptr _wm;
  time_type _resolution;
  pool_ptr _pool;
};
*/
}

#pragma once

#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <string>
#include <memory>

namespace wrtstat {

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
class composite_meter_factory: composite_meter<D>
{
  typedef composite_meter<D> super;
public:
  typedef typename super::meter_fun_t meter_fun_t;
  typedef typename super::time_meter_ptr time_meter_ptr;
  typedef typename super::size_meter_ptr size_meter_ptr;

  
  composite_meter_factory( const time_meter_ptr& tm, const size_meter_ptr& rm, const size_meter_ptr& wm, time_type resolution)
    : super(tm, rm, wm)
    , _resolution(resolution)
  {}
  
  std::shared_ptr< composite_meter<D> > create(size_type size) const
  {
    time_type now_ts = aggregator::now(_resolution);
    return super::clone(now_ts, size);
  }
  
private:
  time_type _resolution;
};

}

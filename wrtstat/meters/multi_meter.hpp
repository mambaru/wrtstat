#pragma once

#include <wrtstat/meters/composite_meter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace wrtstat {

template< typename MeterType>
class multi_meter
{
public:
  typedef multi_meter<MeterType> self_type;
  typedef std::shared_ptr<self_type> self_ptr;

  typedef MeterType meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef std::vector<meter_ptr> meter_list;

  ~multi_meter()
  {
    for (auto& p : _meters )
      if ( p!=nullptr )
        p.reset();
    _meters.clear();
  }

  void reserve(size_t s)
  {
    _meters.reserve(s);
  }
  
  void push_back(meter_ptr p)
  {
    if ( p!=nullptr )
      _meters.push_back(p);
  }

  void reset()
  {
    for (auto p : _meters )
      if ( p!=nullptr )
        p->reset();
  }
  
  void set_write_size(size_type size) 
  {
    for (auto p : _meters )
      if ( p!=nullptr )
        p->set_write_size(size);
  }

  template<typename... A>
  self_ptr clone(time_type now, A... args) const
  {
    auto m = std::make_shared<self_type>();
    m->_meters.reserve(this->_meters.size());
    for (auto p : _meters )
    {
      if ( p!=nullptr )
        m->_meters.push_back( p->clone(now, args...) );
    }
    return  m;
  }

private:
  meter_list _meters;
};

template< typename MeterType>
class multi_meter_factory: multi_meter<MeterType>
{
  typedef multi_meter<MeterType> super;
public:
  typedef typename super::meter_fun_t meter_fun_t;
  typedef typename super::time_meter_ptr time_meter_ptr;
  typedef typename super::size_meter_ptr size_meter_ptr;

  template<typename... A>
  std::shared_ptr< multi_meter<MeterType> > create(A... args) const
  {
    time_type now_ts = aggregator::now(_resolution);
    return super::clone(now_ts, args...);
  }
  
private:
  time_type _resolution;
};


}

#pragma once

#include <wrtstat/meters/composite_meter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace wrtstat {

template< typename MeterType /*typename D, typename MeterType = composite_meter<D>*/ >
class multi_meter
{
public:
  //typedef D duration_type;
  typedef multi_meter</*D,*/ MeterType> self_type;
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
  self_ptr clone(time_type now, A... args/*size_t count, size_t size*/) const
  {
    auto m = std::make_shared<self_type>();
    m->_meters.reserve(this->_meters.size());
    for (auto p : _meters )
    {
      if ( p!=nullptr )
        m->_meters.push_back( p->clone(now, args.../*count, size*/) );
    }
    return  m;
  }

private:
  meter_list _meters;
};

}

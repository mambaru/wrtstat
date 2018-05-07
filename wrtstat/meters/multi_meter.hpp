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
  typedef MeterType meter_type;
  typedef std::vector< meter_type > meter_list;

  multi_meter() = default;
  multi_meter( const multi_meter& ) = delete;
  multi_meter& operator=( const multi_meter& ) = delete;
  multi_meter( multi_meter&& ) = default;
  multi_meter& operator=( multi_meter&& ) = default;

  
  ~multi_meter()
  {
    _meters.clear();
  }

  void reserve(size_t s)
  {
    _meters.reserve(s);
  }
  
  void push_back(meter_type p)
  {
    _meters.push_back( std::move(p) );
  }

  void reset()
  {
    for (auto& p : _meters )
      p.reset();
  }
  
  void set_write_size(size_type size) 
  {
    for (auto& p : _meters )
      p.set_write_size(size);
  }

  void set_read_size(size_type size) 
  {
    for (auto& p : _meters )
      p.set_read_size(size);
  }

  
  template<typename... A>
  multi_meter<MeterType> clone(time_type now, A... args) const
  {
    multi_meter<MeterType> m;
    m._meters.reserve(this->_meters.size());
    for (auto& p : _meters )
    {
      m._meters.push_back( std::move( p.clone(now, args...) ) );
    }
    return  std::move(m);
  }
  
  
private:
  meter_list _meters;
};

template< typename MeterFactory>
class multi_meter_factory
{
public:
  typedef MeterFactory factory_type;
  typedef typename factory_type::meter_type meter_type;
  
  void reserve(size_t value)
  {
    _factory_list.reserve(value);
  }
  void push_back(factory_type factory)
  {
    _factory_list.push_back(factory);
  }

  template<typename... A>
  multi_meter<meter_type> create(A... args) const
  {
    multi_meter<meter_type> m;
    m.reserve(_factory_list.size());
    for (auto& f : _factory_list)
    {
      m.push_back( f.create(args...) );
    }
    return m;
  }
  
  template<typename... A>
  std::shared_ptr<multi_meter<meter_type> > create_shared(A... args) const
  {
    std::shared_ptr<multi_meter<meter_type> > m = std::make_shared< multi_meter<meter_type> >();
    m->reserve(_factory_list.size());
    for (auto& f : _factory_list)
    {
      m->push_back( f.create(args...) );
    }
    return m;
  }
private:
  std::vector<factory_type> _factory_list;
};


}

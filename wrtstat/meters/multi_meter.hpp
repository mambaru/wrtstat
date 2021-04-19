#pragma once

#include <wrtstat/meters/composite_meter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace wrtstat {

template< typename PointType>
class multi_point
{
public:
  typedef multi_point<PointType> self_type;
  typedef PointType point_type;
  typedef std::vector< point_type > point_list;

  multi_point() = default;
  multi_point( const multi_point& ) = delete;
  multi_point& operator=( const multi_point& ) = delete;
  multi_point( multi_point&& ) = default;
  multi_point& operator=( multi_point&& ) = default;

  
  ~multi_point()
  {
    _points.clear();
  }

  void reserve(size_t s)
  {
    _points.reserve(s);
  }
  
  void push_back(point_type p)
  {
    _points.push_back( std::move(p) );
  }

  void reset()
  {
    for (auto& p : _points )
      p.reset();
  }
  
  void set_write_size(value_type size) 
  {
    for (auto& p : _points )
      p.set_write_size(size);
  }

  void set_read_size(value_type size) 
  {
    for (auto& p : _points )
      p.set_read_size(size);
  }

  
  template<typename... A>
  multi_point<point_type> clone(time_type now, A... args) const
  {
    multi_point<point_type> m;
    m._points.reserve(this->_points.size());
    for (const auto& p : _points )
    {
      m._points.push_back( std::move( p.clone(now, args...) ) );
    }
    return  std::move(m);
  }
  
  
private:
  point_list _points;
};

template< typename MeterType>
class multi_meter
{
public:
  typedef MeterType meter_type;
  typedef typename meter_type::point_type point_type;
  
  void reserve(size_t value)
  {
    _meters.reserve(value);
  }
  void push_back(meter_type m)
  {
    _meters.push_back(m);
  }
  
  size_t size() const 
  {
    return _meters.size();
  }

  template<typename... A>
  multi_point<point_type> create(A... args) const
  {
    multi_point<point_type> m;
    m.reserve(_meters.size());
    for (const auto& f : _meters)
    {
      m.push_back( f.create( std::forward<A>(args)...) );
    }
    return m;
  }
  
  template<typename... A>
  std::shared_ptr<multi_point<point_type> > create_shared(A... args) const
  {
    std::shared_ptr<multi_point<point_type> > m = std::make_shared< multi_point<point_type> >();
    m->reserve(_meters.size());
    for (const auto& f : _meters)
    {
      m->push_back( f.create(std::forward<A>(args)...) );
    }
    return m;
  }
private:
  std::vector<meter_type> _meters;
};


}

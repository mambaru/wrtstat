#pragma once

#include <wrtstat/pair_meter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace wrtstat {

template<typename D>
class multi_meter
{
public:
  typedef D duration_type;
  typedef multi_meter<D> self_type;
  typedef std::shared_ptr<self_type> self_ptr;

  typedef pair_meter<D> meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef std::vector<meter_ptr> meter_list;

  typedef typename meter_type::time_type time_type;
  typedef typename meter_type::size_type size_type;

  void push_back(meter_ptr p)
  {
    _meters.push_back(p);
  }

  void reset()
  {
    for (auto p : _meters )
      p->reset();
  }

  self_ptr clone(time_type now, size_type size) const
  {
    auto m = std::make_shared<self_type>();
    m->_meters.reserve(this->_meters.size());
    for (auto p : _meters )
    {
      m->_meters.push_back( p->clone(now, size) );
    }
    return  m;
  }

private:
  meter_list _meters;
};

}

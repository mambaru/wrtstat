#pragma once

#include <wrtstat/time_meter.hpp>
#include <string>

namespace wrtstat {

template<typename D>
class pair_meter
{
public:
  typedef pair_meter<D> self_type;
  typedef std::shared_ptr<self_type> self_ptr;
  typedef D duration_type;
  typedef time_meter< duration_type > time_meter_type;
  typedef size_meter size_meter_type;

  typedef typename time_meter_type::time_type  time_type;
  typedef typename size_meter_type::size_type size_type;
  typedef std::shared_ptr<time_meter_type> time_meter_ptr;
  typedef std::shared_ptr<size_meter_type> size_meter_ptr;

  pair_meter(time_meter_ptr rm, size_meter_ptr sm)
    : _rate_meter(rm)
    , _size_meter(sm)
  {
  }

  self_ptr clone(time_type now, size_t size) const
  {
    time_meter_ptr rate_meter;
    size_meter_ptr size_meter;
    if ( _rate_meter!=nullptr )
      rate_meter = _rate_meter->clone(now, 1);
    if ( _size_meter!=nullptr )
      size_meter = _size_meter->clone(now, size);

    return std::make_shared<pair_meter>(rate_meter, size_meter);
  }

private:
  time_meter_ptr _rate_meter;
  size_meter_ptr _size_meter;
};

}

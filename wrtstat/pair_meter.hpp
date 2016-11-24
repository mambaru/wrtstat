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
  typedef ::wrtstat::time_meter< duration_type > meter_type;

  typedef typename meter_type::time_type  time_type;
  typedef typename meter_type::size_type size_type;
  typedef std::shared_ptr<meter_type> meter_ptr;

  pair_meter(meter_ptr rm, meter_ptr sm)
    : _rate_meter(rm)
    , _size_meter(sm)
  {
  }

  self_ptr clone(time_type now, size_t size) const
  {
    meter_ptr rate_meter;
    meter_ptr size_meter;
    if ( _rate_meter!=nullptr )
      rate_meter = _rate_meter->clone(now, 1);
    if ( _size_meter!=nullptr )
      size_meter = _size_meter->clone(now, size);

    return std::make_shared<pair_meter>(rate_meter, size_meter);
  }

private:
  meter_ptr _rate_meter;
  meter_ptr _size_meter;
};

}

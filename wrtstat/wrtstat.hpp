#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/meter_manager.hpp>
#include <memory>

namespace wrtstat{

using composite_multi_meter = multi_meter< composite_meter<std::chrono::microseconds> >;
using time_multi_meter = multi_meter< time_meter<std::chrono::microseconds> >;
using value_multi_meter = multi_meter< value_meter >;
using size_multi_meter = multi_meter< size_meter > ;

class wrtstat
  : public meter_manager
{
public:
  typedef wrtstat_options options_type;
  explicit wrtstat(const options_type& opt):meter_manager(opt){}
};

}

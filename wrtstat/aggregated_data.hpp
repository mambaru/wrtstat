#pragma once
#include <wrtstat/reduced_data.hpp>
#include <memory>
#include <functional>

namespace wrtstat {

struct aggregated_perc
{
  value_type perc50  = 0;
  value_type perc80  = 0;
  value_type perc95  = 0;
  value_type perc99  = 0;
  value_type perc100 = 0;
};

struct aggregated_info
  : reduced_info
  , aggregated_perc
{
  aggregated_info() = default;
};

  
struct aggregated_data
  : reduced_data
  , aggregated_perc
{
  typedef std::unique_ptr<aggregated_data> ptr;
  typedef std::function< void(ptr) > handler;
};

}

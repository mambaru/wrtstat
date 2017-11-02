#pragma once
#include <wrtstat/reduced_data.hpp>
#include <memory>
#include <functional>

namespace wrtstat {

struct aggregated_data
  : reduced_data
{
  typedef reduced_data::value_type value_type;
  value_type perc50  = 0;
  value_type perc80  = 0;
  value_type perc95  = 0;
  value_type perc99  = 0;
  value_type perc100 = 0;

  typedef std::unique_ptr<aggregated_data> ptr;
  typedef std::function< void(ptr) > handler;
};

}

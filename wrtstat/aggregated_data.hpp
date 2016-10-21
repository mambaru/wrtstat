#pragma once
#include <wrtstat/reduced_data.hpp>

namespace wrtstat {

struct aggregated_data: reduced_data
{
  typedef reduced_data::value_type value_type;
  value_type perc50;
  value_type perc80;
  value_type perc95;
  value_type perc99;
  value_type perc100;
};

}

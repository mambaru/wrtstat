#pragma once
#include <wrtstat/aggregator/api/types.hpp>

namespace wrtstat {

struct aggregated_perc
{
  value_type perc50  = 0;
  value_type perc80  = 0;
  value_type perc95  = 0;
  value_type perc99  = 0;
  value_type perc100 = 0;
};


}

#pragma once

#include <wrtstat/aggregator_options.hpp>

namespace wrtstat {

struct manager_options
  : aggregator_options
{
  // количество массивов размером reducer_limit
  size_t data_pool = 0;
};

}

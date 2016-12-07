#pragma once

#include <wrtstat/aggregator_options.hpp>

namespace wrtstat {

struct manager_options
  : aggregator_options
{
  size_t pool = 0;
};

}

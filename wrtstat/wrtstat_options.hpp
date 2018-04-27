#pragma once

#include <wrtstat/manager/manager_options.hpp>

namespace wrtstat {

struct wrtstat_options
  : aggregator_options
{
  std::vector<std::string> prefixes;
  size_t pool_size = 0;
};

}

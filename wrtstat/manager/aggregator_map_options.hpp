#pragma once
#include <wrtstat/aggregator_options.hpp>
#include <cstddef>
                  
namespace wrtstat {

struct aggregator_map_options: aggregator_options
{
  size_t pool_size = 0;
};


}

#pragma once
#include <cstddef>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct reducer_options
{
  enum class mode{
    sorting = 1, 
    nth = 2,
    adapt = 3
  };
  size_type reducer_limit = 1024;
  size_type reducer_levels = 16;
  mode reducer_mode = mode::adapt;
  mode initial_mode = mode::adapt;
};

}

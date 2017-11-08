#pragma once
#include <cstddef>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct reducer_options
{
  size_type reducer_limit = 1024;
  size_type reducer_levels = 16;
};

}

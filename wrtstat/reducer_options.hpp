#pragma once
#include <cstddef>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct reducer_options
{
  types::size_type reducer_limit = 1024;
  types::size_type reducer_levels = 16;
};

}

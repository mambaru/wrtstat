#pragma once
#include <cstddef>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct reducer_options
{
  types::size_type limit = 1024;
  types::size_type levels = 16;
};

}

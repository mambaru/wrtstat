#pragma once
#include <wrtstat/reducer_options.hpp>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct separator_options
  : reducer_options
{
  types::time_type step_ts = 0;
};

}

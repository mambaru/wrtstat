#pragma once
#include <wrtstat/separator_options.hpp>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct aggregator_options
  : separator_options
{
  types::size_type outgoing_reduced_size = 128;
};

}

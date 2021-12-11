#pragma once

#include <wrtstat/aggregator/options/separator_options.hpp>
#include <functional>
namespace wrtstat {

struct aggregator_options
  : separator_options
{  
  size_type outgoing_reduced_size = 128;
};

}

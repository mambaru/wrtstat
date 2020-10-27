#pragma once
#include <wrtstat/aggregator/api/reduced_data.hpp>
#include <wrtstat/aggregator/api/aggregated_perc.hpp>

#include <memory>
#include <functional>

namespace wrtstat {
 
struct aggregated_data
  : reduced_data
  , aggregated_perc
{
  typedef std::unique_ptr<aggregated_data> ptr;
  typedef std::function< void(ptr) > handler;
};

}

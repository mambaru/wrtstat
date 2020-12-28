#pragma once

#include <wrtstat/aggregator/options/aggregator_options.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>

namespace wrtstat {

struct wrtstat_options
  : aggregator_options
{
  //typedef aggregated_data::handler aggregated_handler;
  typedef std::function<void(const std::string& name, aggregated_data::ptr)> named_aggregated_handler;
  std::vector<std::string> prefixes;
  size_t pool_size = 0;

  named_aggregated_handler handler = nullptr;
  id_t id_init = 0;
  id_t id_step = 1;
};

}

#pragma once

#include <wrtstat/aggregator/options/registry_options.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>

namespace wrtstat {

struct wrtstat_options
  : registry_options
{
  typedef std::function<void(const std::string& name, aggregated_data::ptr)> named_aggregated_handler;
  std::vector<std::string> prefixes;
  named_aggregated_handler handler = nullptr;
};

}

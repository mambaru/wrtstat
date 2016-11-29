#pragma once

#include <wrtstat/manager_options.hpp>

namespace wrtstat {

struct wrtstat_options
  : manager_options
{
  std::vector<std::string> prefixes;
};

}

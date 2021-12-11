#pragma once

#include <wrtstat/aggregator/options/aggregator_options.hpp>
#include <wrtstat/aggregator/api/reduced_data.hpp>
#include <functional>

namespace wrtstat {

struct registry_options
  : aggregator_options
{
  typedef std::function<bool(const std::string& name, reduced_data* rd)> initializer_fun_t;

  size_t pool_size = 0;
  
  // non serialize 
  id_t id_init = 0;
  id_t id_step = 1;
  initializer_fun_t initializer = nullptr;
};

}

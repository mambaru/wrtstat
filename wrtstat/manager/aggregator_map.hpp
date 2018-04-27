#pragma once
#include <wrtstat/aggregator.hpp>
#include <wrtstat/allocator.hpp>
#include <wrtstat/manager/pool.hpp>
#include <wrtstat/manager/aggregator_map_options.hpp>
#include <wrtstat/manager/mutex/empty_mutex.hpp>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace wrtstat {

class aggregator_map
{
public:
  typedef aggregator_base aggregator_type;
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef aggregator_options options_type;
  typedef aggregator_base::aggregated_handler aggregated_handler;
  typedef aggregator_base::aggregated_ptr aggregated_ptr;
  typedef std::unordered_map<std::string, aggregator_ptr> aggregator_map_t;
  
  explicit aggregator_map(const options_type& opt = options_type(), size_t pool_size = 0 );

  bool add( const std::string& name, const reduced_data& v, aggregated_handler handler);
  
private:
  aggregator_map_t _aggregator_map;
  options_type _opt;
  pool<empty_mutex> _pool;
};
}

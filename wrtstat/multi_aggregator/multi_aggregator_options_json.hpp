#pragma once

#include <wrtstat/multi_aggregator/multi_aggregator_options.hpp>
#include <wrtstat/aggregator/options/aggregator_options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{
  
struct multi_aggregator_options_options_json
{
  JSON_NAME(pool_size)
  JSON_NAME(hash_size)

  typedef wjson::object<
    multi_aggregator_options,
    wjson::member_list<
      wjson::base<aggregator_options_json>,
      wjson::member< n_pool_size, multi_aggregator_options, size_t, &multi_aggregator_options::pool_size>,
      wjson::member< n_hash_size, multi_aggregator_options, size_t, &multi_aggregator_options::hash_size>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

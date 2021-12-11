#pragma once

#include <wrtstat/aggregator/options/registry_options.hpp>
#include <wrtstat/aggregator/options/aggregator_options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{

struct registry_options_json
{
  JSON_NAME(pool_size)

  typedef wjson::object<
    registry_options,
    wjson::member_list<
      wjson::base<aggregator_options_json>,
      wjson::member<n_pool_size, registry_options, size_t, &registry_options::pool_size>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

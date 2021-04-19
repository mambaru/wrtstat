#pragma once

#include <wrtstat/aggregator/options/reducer_options.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{
  
struct reducer_options_json
{
  JSON_NAME(reducer_limit)
  JSON_NAME(reducer_levels)
  
  typedef wjson::object<
    reducer_options,
    wjson::member_list<
      wjson::member< n_reducer_limit, reducer_options, size_type, &reducer_options::reducer_limit>,
      wjson::member< n_reducer_levels, reducer_options, size_type, &reducer_options::reducer_levels>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

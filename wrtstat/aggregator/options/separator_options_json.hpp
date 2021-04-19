#pragma once

#include <wrtstat/aggregator/options/separator_options.hpp>
#include <wrtstat/aggregator/options/resolution_json.hpp>
#include <wrtstat/aggregator/options/reducer_options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{

struct separator_options_json
{
  JSON_NAME(aggregation_step_ts)
  JSON_NAME(soiled_start_ts)
  JSON_NAME(resolution)

  typedef wjson::object<
    separator_options,
    wjson::member_list<
      wjson::base<reducer_options_json>,
      wjson::member< n_aggregation_step_ts, separator_options, time_type, &separator_options::aggregation_step_ts>,
      wjson::member< n_soiled_start_ts, separator_options, time_type, &separator_options::soiled_start_ts>,
      wjson::member< n_resolution, separator_options, resolutions, &separator_options::resolution, resolution_json>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{
  
struct wrtstat_options_json
{
  JSON_NAME(outgoing_reduced_size)
  JSON_NAME(aggregation_step_ts)
  JSON_NAME(reducer_limit)
  JSON_NAME(reducer_levels)
  JSON_NAME(pool_size)
  JSON_NAME(prefixes)

  typedef wjson::object<
    wrtstat_options,
    wjson::member_list<
      wjson::member< n_outgoing_reduced_size,  aggregator_options, size_type, &aggregator_options::outgoing_reduced_size>,
      wjson::member< n_aggregation_step_ts, separator_options, time_type, &separator_options::aggregation_step_ts>,
      wjson::member< n_reducer_limit, reducer_options, size_type, &reducer_options::reducer_limit>,
      wjson::member< n_reducer_levels, reducer_options, size_type, &reducer_options::reducer_levels>,
      wjson::member< n_pool_size, wrtstat_options, size_t, &wrtstat_options::pool_size>,
      wjson::member< n_prefixes, wrtstat_options, std::vector<std::string>, &wrtstat_options::prefixes,
                     wjson::vector_of_strings<> >
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{
  
/*
enum class resolutions
{
  none = 0,
  seconds = 1, 
  milliseconds = 1000,
  microseconds = 1000000,
  nanoseconds = 1000000000
};

*/

struct resolutions_json
{
  JSON_NAME(none)
  JSON_NAME(seconds)
  JSON_NAME(milliseconds)
  JSON_NAME(microseconds)
  JSON_NAME(nanoseconds)
  typedef wjson::enumerator<
    resolutions,
    wjson::member_list<
      wjson::enum_value<n_none, resolutions, resolutions::none>,
      wjson::enum_value<n_seconds, resolutions, resolutions::seconds>,
      wjson::enum_value<n_milliseconds, resolutions, resolutions::milliseconds>,
      wjson::enum_value<n_microseconds, resolutions, resolutions::microseconds>,
      wjson::enum_value<n_nanoseconds, resolutions, resolutions::nanoseconds>
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


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


struct separator_options_json
{
  JSON_NAME(aggregation_step_ts)
  JSON_NAME(resolutions)

  typedef wjson::object<
    separator_options,
    wjson::member_list<
      wjson::base<reducer_options_json>,
      wjson::member< n_aggregation_step_ts, separator_options, time_type, &separator_options::aggregation_step_ts>,
      wjson::member< n_resolutions, separator_options, resolutions, &separator_options::resolution, resolutions_json>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct aggregator_options_json
{
  JSON_NAME(outgoing_reduced_size)

  typedef wjson::object<
    aggregator_options,
    wjson::member_list<
      wjson::base<separator_options_json>,
      wjson::member< n_outgoing_reduced_size,  aggregator_options, size_type, &aggregator_options::outgoing_reduced_size>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};
  
struct wrtstat_options_json
{
  JSON_NAME(pool_size)
  JSON_NAME(prefixes)

  typedef wjson::object<
    wrtstat_options,
    wjson::member_list<
      wjson::base<aggregator_options_json>,
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

struct agmap_options_json
{
  JSON_NAME(pool_size)
  JSON_NAME(hash_size)

  typedef wjson::object<
    agmap_options,
    wjson::member_list<
      wjson::base<aggregator_options_json>,
      wjson::member< n_pool_size, agmap_options, size_t, &agmap_options::pool_size>,
      wjson::member< n_hash_size, agmap_options, size_t, &agmap_options::hash_size>
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

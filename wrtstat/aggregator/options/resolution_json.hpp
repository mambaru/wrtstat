#pragma once

#include <wrtstat/aggregator/api/types.hpp>
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

struct resolution_json
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

}

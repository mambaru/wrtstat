#pragma once

#include <wrtstat/aggregator/options/aggregator_options.hpp>
#include <wrtstat/aggregator/options/separator_options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{

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

}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/api/json/reduced_data_json_compact.hpp>  
#include <wrtstat/aggregator/api/json/aggregated_perc_json_compact.hpp>

namespace wrtstat{

struct aggregated_data_json_compact
{
  typedef wjson::object_array<
    aggregated_data,
    wjson::member_list<
      reduced_data_json_compact::head_list,
      aggregated_perc_json_compact::member_list,
      reduced_data_json_compact::tail_list
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};


}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/api/json/reduced_data_json.hpp>  
#include <wrtstat/aggregator/api/json/aggregated_perc_json.hpp>

namespace wrtstat{

struct aggregated_data_json
{
  typedef wjson::object<
    aggregated_data,
    wjson::member_list<
      reduced_data_json::head_list,
      aggregated_perc_json::member_list,
      reduced_data_json::tail_list
    >
  > meta;
  typedef meta::target target;
  typedef meta::member_list member_list;
  typedef meta::serializer serializer;
};


}

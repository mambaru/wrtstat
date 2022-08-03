//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/aggregated_info.hpp>
#include <wrtstat/aggregator/api/json/reduced_info_json.hpp>  
#include <wrtstat/aggregator/api/json/aggregated_perc_json.hpp> 

namespace wrtstat{

struct aggregated_info_json
{
  typedef wjson::object<
    aggregated_info,
    wjson::member_list<
      reduced_info_json::head_list,
      aggregated_perc_json::member_list,
      reduced_info_json::tail_list
    >
  > meta;
  typedef meta::target target;
  typedef meta::member_list member_list;
  typedef meta::serializer serializer;
};

}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/api/aggregated_info.hpp>
#include <wrtstat/aggregator/api/json/reduced_info_json.hpp>  

namespace wrtstat{
  
struct aggregated_perc_json
{  
  JSON_NAME(perc50)
  JSON_NAME(perc80)
  JSON_NAME(perc95)
  JSON_NAME(perc99)
  JSON_NAME(perc100)
  
  typedef wjson::object<
    aggregated_perc,
    wjson::member_list<
      wjson::member<n_perc50,   aggregated_perc,   value_type,  &aggregated_perc::perc50>,
      wjson::member<n_perc80,   aggregated_perc,   value_type,  &aggregated_perc::perc80>,
      wjson::member<n_perc95,   aggregated_perc,   value_type,  &aggregated_perc::perc95>,
      wjson::member<n_perc99,   aggregated_perc,   value_type,  &aggregated_perc::perc99>,
      wjson::member<n_perc100,  aggregated_perc,   value_type,  &aggregated_perc::perc100>
    >
  > meta;
  typedef meta::target target;
  typedef meta::member_list member_list;
  typedef meta::serializer serializer;
};

  
}

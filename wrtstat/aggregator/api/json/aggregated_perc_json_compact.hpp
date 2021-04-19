//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/aggregated_perc.hpp>

namespace wrtstat{
 
struct aggregated_perc_json_compact
{
  typedef wjson::object_array<
    aggregated_perc,
    wjson::member_list<
      wjson::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc50>,
      wjson::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc80>,
      wjson::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc95>,
      wjson::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc99>,
      wjson::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc100>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

}

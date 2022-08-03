//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/aggregated_info.hpp>

#include <wrtstat/aggregator/api/json/reduced_info_json_compact.hpp>
#include <wrtstat/aggregator/api/json/aggregated_perc_json_compact.hpp>
#include <wrtstat/aggregator/api/json/reduced_info_json_compact.hpp>

namespace wrtstat{

struct aggregated_info_json_compact
{
  typedef wjson::object_array<
    aggregated_info,
    wjson::member_list<
      reduced_info_json_compact::head_list,
      aggregated_perc_json_compact::member_list,
      reduced_info_json_compact::tail_list
    >
  > meta;
  typedef meta::target target;
  typedef meta::member_list member_list;
  typedef meta::serializer serializer;
};

}

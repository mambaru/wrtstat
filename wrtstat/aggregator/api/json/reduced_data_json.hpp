//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wrtstat/aggregator/api/json/reduced_info_json.hpp>
#include <wrtstat/aggregator/api/reduced_data.hpp>
#include <wjson/wjson.hpp>

namespace wrtstat{

struct reduced_data_json
{
  JSON_NAME(data)

  typedef wjson::vector_of< wjson::value<value_type>, 128 > data_json;
  typedef reduced_info_json::head_list head_list;
  typedef fas::type_list_n<
    reduced_info_json::tail_list,
    wjson::member<n_data,  reduced_data,  data_type,  &reduced_data::data, data_json>
  >::type tail_list;
  
  typedef wjson::object<
    reduced_data,
    wjson::member_list<
       head_list,
       tail_list
      >
  > meta;
  typedef meta::target target;
  typedef meta::member_list member_list;
  typedef meta::serializer serializer;
};
  
}

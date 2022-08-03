//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/reduced_info.hpp>
  
namespace wrtstat{
    

struct reduced_info_json_compact
{
  // Разбили для совместимости со старым api на два списка 
  // (см. в aggregated_info_json_compact между втавим перцентили)
  
  typedef fas::type_list_n<
    wjson::member_array<reduced_info,  time_type,  &reduced_info::ts>,
    wjson::member_array<reduced_info,  value_type, &reduced_info::avg>,
    wjson::member_array<reduced_info,  size_type,  &reduced_info::count>
  >::type head_list;

  typedef fas::type_list_n<
    wjson::member_array<reduced_info,  value_type, &reduced_info::min>,
    wjson::member_array<reduced_info,  value_type, &reduced_info::max>,
    wjson::member_array<reduced_info,  size_type,  &reduced_info::lossy>
  >::type tail_list;
  
  typedef wjson::object_array<
    reduced_info,
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

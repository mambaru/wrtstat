//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wjson/wjson.hpp>
#include <wrtstat/aggregator/api/reduced_info.hpp>
  
namespace wrtstat{

struct reduced_info_json
{
  JSON_NAME(ts)
  JSON_NAME(avg)
  JSON_NAME(count)
  JSON_NAME(lossy)
  JSON_NAME(min)
  JSON_NAME(max)

  typedef fas::type_list_n<
    wjson::member<n_ts,    reduced_info,  time_type, &reduced_info::ts>,
    wjson::member<n_avg,   reduced_info,  value_type, &reduced_info::avg>,
    wjson::member<n_count, reduced_info,  size_type,  &reduced_info::count>
  >::type head_list;
  
  typedef fas::type_list_n<
    wjson::member<n_lossy, reduced_info,  size_type,  &reduced_info::lossy>,
    wjson::member<n_min,   reduced_info,  value_type, &reduced_info::min>,
    wjson::member<n_max,   reduced_info,  value_type, &reduced_info::max>
  >::type tail_list;
  
  typedef wjson::object<
    reduced_info,
    wjson::member_list<
      head_list,
      tail_list
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

}

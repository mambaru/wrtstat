//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/api/push.hpp>
#include "wrtstat/aggregator/api/json/aggregated_data_json.hpp"

namespace wrtstat{

namespace request {
  
  struct push_json
  {
    JSON_NAME(name)
 
    typedef wjson::object<
      push,
      wjson::member_list<
        wjson::member<n_name, push, std::string, &push::name>,
        wjson::base<aggregated_data_json>
      >
    > meta;
    
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct push_json
  {
    JSON_NAME(status)
    
    typedef wjson::object<
      push,
      wjson::member_list<
        wjson::member<n_status, push, bool, &push::status>
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}
 
}

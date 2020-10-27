//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/api/push.hpp>
#include "wrtstat/aggregator/api/json/aggregated_data_json_compact.hpp"

namespace wrtstat{

namespace request {
  
  struct push_json_compact
  {
 
    typedef wjson::object_array<
      push,
      wjson::member_list<
        wjson::member_array<push, std::string, &push::name>,
        wjson::base<aggregated_data_json_compact>
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct push_json_compact
  {
    typedef wjson::object_array<
      push,
      wjson::member_list<
        wjson::member_array<push, bool, &push::status>
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

  
}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/api/push_json_compact.hpp>
#include <wrtstat/api/multi_push.hpp>
#include <wjson/wjson.hpp>

namespace wrtstat {

namespace request {

  struct multi_push_json_compact
  {
    typedef wjson::object_array<
      multi_push,
      wjson::member_list<
        wjson::member_array<multi_push, std::string, &multi_push::sep >,
        wjson::member_array<multi_push, std::vector<std::string>, &multi_push::legend, wjson::vector_of_strings<16> >,
        wjson::member_array<multi_push, std::deque<push>, &multi_push::data, wjson::deque_of<push_json_compact> >
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
  };
}

namespace response
{
  struct multi_push_json_compact
  {
    typedef wjson::object_array<
      multi_push,
      wjson::member_list<
        wjson::member_array<multi_push, bool, &multi_push::status>,
        wjson::member_array<multi_push, std::string, &multi_push::error >
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
  };
}

}


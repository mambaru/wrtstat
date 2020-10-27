//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/api/push_json.hpp>
#include <wrtstat/api/multi_push.hpp>
#include <wjson/wjson.hpp>

namespace wrtstat {

namespace request {

  struct multi_push_json
  {
    JSON_NAME(sep)
    JSON_NAME(legend)
    JSON_NAME(data)

    typedef wjson::object<
      multi_push,
      wjson::member_list<
        wjson::member<n_sep, multi_push, std::string, &multi_push::sep >,
        wjson::member<n_legend, multi_push, std::vector<std::string>, &multi_push::legend, wjson::vector_of_strings<16> >,
        wjson::member<n_data, multi_push, std::deque<push>, &multi_push::data, wjson::deque_of<push_json> >
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct multi_push_json
  {
    JSON_NAME(status)
    JSON_NAME(error)

    typedef wjson::object<
      multi_push,
      wjson::member_list<
        wjson::member<n_status, multi_push, bool, &multi_push::status>,
        wjson::member<n_error, multi_push, std::string, &multi_push::error >
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}


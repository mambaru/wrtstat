//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/wjson.hpp>
#include <wrtstat/api/del.hpp>

namespace wrtstat{

namespace request {

  struct del_json
  {
    JSON_NAME(names)

    typedef wjson::object<
      del,
      wjson::member_list<
        wjson::member<n_names, del, std::vector<std::string>, &del::names, wjson::vector_of_strings<> >
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
  };
}

namespace response
{
  struct del_json
  {
    typedef wjson::object<
      del,
      wjson::member_list<
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
  };
}

}

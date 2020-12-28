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
  
  struct del_json_compact
  {
    typedef wjson::object_array<
      del,
      wjson::member_list<
        wjson::member_array<del, std::string, &del::regexp>,
        wjson::member_array<del, bool, &del::dryrun>,
        wjson::member_array<del, bool, &del::get_names>
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct del_json_compact
  {
    typedef wjson::object_array<
      del,
      wjson::member_list<
        wjson::member_array<del, size_t, &del::count>,
        wjson::member_array<del, std::string, &del::error>,
        wjson::member_array<del, std::vector<std::string>, &del::names, wjson::vector_of_strings<> >
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}

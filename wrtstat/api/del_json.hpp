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
    JSON_NAME(regexp)
    JSON_NAME(dryrun)
    JSON_NAME(get_names)
    
    typedef wjson::object<
      del,
      wjson::member_list<
        wjson::member<n_regexp, del, std::string, &del::regexp>,
        wjson::member<n_dryrun, del, bool, &del::dryrun>,
        wjson::member<n_get_names, del, bool, &del::get_names>
        >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct del_json
  {
    JSON_NAME(count)
    JSON_NAME(error)
    JSON_NAME(names)
    
    typedef wjson::object<
      del,
      wjson::member_list<
        wjson::member<n_count, del, size_t, &del::count>,
        wjson::member<n_error, del, std::string, &del::error>,
        wjson::member<n_names, del, std::vector<std::string>, &del::names, wjson::vector_of_strings<> >
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}

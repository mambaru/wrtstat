#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/aggregator/options/registry_options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>

namespace wrtstat{

struct wrtstat_options_json
{
  /*JSON_NAME(prefixes)*/

  typedef wjson::object<
    wrtstat_options,
    wjson::member_list<
      wjson::base<registry_options_json>/*,
      wjson::member< n_prefixes, wrtstat_options, std::vector<std::string>, &wrtstat_options::prefixes,
                     wjson::vector_of_strings<> >*/
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}

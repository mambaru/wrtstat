//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/multi_packer/packer_options.hpp>
#include <wjson/wjson.hpp>

namespace wrtstat {

struct packer_options_json
{
  JSON_NAME(push_limit)
  JSON_NAME(data_limit) 
  JSON_NAME(json_limit)
  JSON_NAME(json_compact)
  JSON_NAME(name_sep)
  JSON_NAME(name_compact)
  
  typedef wjson::object<
    packer_options,
    wjson::member_list<
      wjson::member<n_push_limit, packer_options, size_t, &packer_options::push_limit >, 
      wjson::member<n_data_limit, packer_options, size_t, &packer_options::data_limit>,  
      wjson::member<n_json_limit, packer_options, size_t, &packer_options::json_limit>, 
      wjson::member<n_json_compact, packer_options, bool, &packer_options::json_compact >, 
      wjson::member<n_name_sep, packer_options, std::string, &packer_options::name_sep>,
      wjson::member<n_name_compact, packer_options, bool, &packer_options::name_compact>
    >
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
  
};

}

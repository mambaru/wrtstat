//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2018
//
// Copyright: See COPYING file that comes with this distribution
//


#include "load_options.hpp"
#include "../wrtstat_options_json.hpp"

#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <fstream>

namespace wrtstat{

bool load_options(const std::string& path_or_json, wrtstat_options* opt, std::string* err)
{
  std::string jsonstr;
  wjson::json_error er;

  auto beg = wjson::parser::parse_space(path_or_json.begin(), path_or_json.end(), &er);
  if ( !wjson::parser::is_object(beg, path_or_json.end()) )
  {
    std::ifstream ifs(path_or_json);
    if ( ifs.good() )
    {
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), std::back_inserter(jsonstr));
    }
    else
    {
      if (err!=nullptr)
        *err = strerror(errno);
      return false;
    }
  }
  else
    jsonstr.assign( beg, path_or_json.end());
 
  wrtstat_options_json::serializer()(*opt, jsonstr.begin(), jsonstr.end(), &er);
  
  if ( !er )
    return true;
  
  if ( err==nullptr )
    return false;
  
  *err = wjson::strerror::message_trace(er, jsonstr.begin(), jsonstr.end());
  
  return false;
}

wrtstat_options load_options(const std::string& path_or_json, std::string* err)
{
  wrtstat_options opt;
  if ( load_options(path_or_json, &opt, err) )
    return opt;
  return wrtstat_options();
}

std::string serialize_options(const wrtstat_options& opt)
{
  std::string res;
  wrtstat_options_json::serializer()(opt, std::back_inserter(res) );
  return res;
}

}

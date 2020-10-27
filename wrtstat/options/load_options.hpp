//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <string>

namespace wrtstat{
  
bool load_options(const std::string& path_or_json, wrtstat_options* opt, std::string* err = nullptr);

wrtstat_options load_options(const std::string& path_or_json, std::string* err = nullptr); 

std::string serialize_options(const wrtstat_options& opt);

}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <string>

namespace wrtstat{
  
bool load(const std::string& src, wrtstat_options* opt, std::string* err = nullptr);

wrtstat_options load(const std::string& src, std::string* err = nullptr); 

std::string dump(const wrtstat_options& opt);

}

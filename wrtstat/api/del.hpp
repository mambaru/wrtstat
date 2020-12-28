//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <memory>
#include <functional>

namespace wrtstat{
  
namespace request
{
  struct del
  {
    std::string regexp;
    bool dryrun = false;
    bool get_names = false;
    typedef std::unique_ptr<del> ptr;
  };
}

namespace response
{
  struct del 
  {
    // false - если не найден
    size_t count = 0;
    std::string error;
    std::vector<std::string> names;
    typedef std::unique_ptr<del> ptr;
    typedef std::function<void(ptr)> handler;
  };
}

}

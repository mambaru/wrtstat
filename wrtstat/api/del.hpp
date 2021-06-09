//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace wrtstat{

namespace request
{
  struct del
  {
    std::vector<std::string> names;
    typedef std::unique_ptr<del> ptr;
  };
}

namespace response
{
  struct del
  {
    typedef std::unique_ptr<del> ptr;
    typedef std::function<void(ptr)> handler;
  };
}

}

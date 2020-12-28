//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <string>

namespace wrtstat{
  
namespace request
{
  struct push: aggregated_data
  {
    std::string name;
    typedef std::unique_ptr<push> ptr;
  };
}

namespace response
{
  struct push
  {
    bool status = true;
    typedef std::unique_ptr<push> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/aggregator/options/aggregator_options.hpp>
#include <string>

namespace wrtstat {

struct multi_aggregator_options
  : aggregator_options
{
  size_t pool_size = 0;
  size_t hash_size = 4096;
};

}

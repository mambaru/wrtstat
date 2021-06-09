#pragma once
#include <wrtstat/aggregator/api/reduced_info.hpp>
#include <functional>

namespace wrtstat {

struct reduced_data: reduced_info
{
  data_type data;
};

}

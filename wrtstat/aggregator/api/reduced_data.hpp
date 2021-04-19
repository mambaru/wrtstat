#pragma once
#include <wrtstat/aggregator/api/reduced_info.hpp>
#include <functional>

namespace wrtstat {

struct reduced_data: reduced_info
{
  data_type data;
  /*typedef std::unique_ptr<reduced_data> ptr;
  typedef std::function< void(ptr) > handler;*/
};

}

#pragma once
#include <wrtstat/aggregator/basic_aggregator.hpp>
#include <algorithm>

namespace wrtstat {

class aggregator
  : public basic_aggregator
  , public std::enable_shared_from_this<aggregator>
{
public:
  typedef basic_aggregator::options_type options_type;

  aggregator(time_type ts_now, const options_type& opt, const allocator& a = allocator());

  simple_pusher_t create_simple_pusher(aggregated_handler handler);

  data_pusher_t create_data_pusher(aggregated_handler handler );

  reduced_pusher_t create_reduced_pusher(aggregated_handler handler );
};

}

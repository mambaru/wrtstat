#include "aggregator.hpp"
#include "basic_aggregator.hpp"

namespace wrtstat {

aggregator::aggregator(time_type ts_now, const options_type& opt, const allocator& a )
  : basic_aggregator(ts_now, opt, a)
{
}

// pusher
aggregator::simple_pusher_t aggregator::create_simple_pusher(aggregated_handler handler )
{
  return basic_aggregator::create_simple_pusher<aggregator>( this->shared_from_this(), handler  );
}

aggregator::data_pusher_t aggregator::create_data_pusher(aggregated_handler handler )
{
  return basic_aggregator::create_data_pusher<aggregator>( this->shared_from_this(), handler  );
}

aggregator::reduced_pusher_t aggregator::create_reduced_pusher(aggregated_handler handler )
{
  return basic_aggregator::create_reduced_pusher<aggregator>( this->shared_from_this(), handler  );
}

}

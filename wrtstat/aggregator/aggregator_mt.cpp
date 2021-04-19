#include "aggregator_mt.hpp"

namespace wrtstat {

aggregator_mt::aggregator_mt(time_type ts_now, const options_type& opt, const allocator& a)
  : basic_aggregator(ts_now, opt, a)
{
}
  
bool aggregator_mt::add(time_type ts_now, value_type v, size_type count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::add(ts_now, v, count);
}

bool aggregator_mt::add(time_type ts_now, const data_type& v, size_type count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::add(ts_now, v, count);
}

bool aggregator_mt::add( const reduced_data& v)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::add(v);
}

bool aggregator_mt::push( time_type ts_now, value_type v, size_type count, aggregated_handler handler)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::push(ts_now, v, count, handler);
}

bool aggregator_mt::push( time_type ts_now, const data_type& v, size_type count, aggregated_handler handler)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::push(ts_now, v, count, handler);
}

bool aggregator_mt::push( const reduced_data& v, aggregated_handler handler)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::push(v, handler);
}

aggregator_mt::aggregated_ptr aggregator_mt::pop()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::pop();
}

aggregator_mt::aggregated_ptr aggregator_mt::force_pop()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::force_pop();
}

aggregator_mt::aggregated_ptr aggregator_mt::aggregate_current()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::aggregate_current();
}

bool aggregator_mt::separate(time_type ts_now, aggregated_handler handler, bool force)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::separate(ts_now, handler, force);
}

void aggregator_mt::enable(bool value)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::enable(value);
}

//

aggregator_mt::simple_pusher_t aggregator_mt::create_simple_pusher(aggregated_handler handler )
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::create_simple_pusher<aggregator_mt>( this->shared_from_this(), handler  );
}

aggregator_mt::data_pusher_t aggregator_mt::create_data_pusher(aggregated_handler handler )
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::create_data_pusher<aggregator_mt>( this->shared_from_this(), handler);
}

aggregator_mt::reduced_pusher_t aggregator_mt::create_reduced_pusher(aggregated_handler handler )
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::create_reduced_pusher<aggregator_mt>( this->shared_from_this(), handler );
}

size_t aggregator_mt::size() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::size();
}

void aggregator_mt::clear(time_type ts_now)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return basic_aggregator::clear( ts_now );
}

}

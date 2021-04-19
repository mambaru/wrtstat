#pragma once
#include <wrtstat/aggregator/basic_aggregator.hpp>
#include <mutex>
namespace wrtstat {

class aggregator_mt
  : private basic_aggregator
  , public std::enable_shared_from_this<aggregator_mt>
{
public:
  typedef std::mutex mutex_type;

  typedef basic_aggregator::reduced_type reduced_type;
  typedef basic_aggregator::reduced_ptr reduced_ptr;
  typedef basic_aggregator::options_type options_type;
  typedef basic_aggregator::aggregated_ptr aggregated_ptr;
  typedef basic_aggregator::aggregated_handler aggregated_handler;
  typedef basic_aggregator::simple_pusher_t simple_pusher_t;
  typedef basic_aggregator::data_pusher_t data_pusher_t;
  typedef basic_aggregator::reduced_pusher_t reduced_pusher_t;

  aggregator_mt(time_type ts_now, const options_type& opt, const allocator& a = allocator() );

  bool add(time_type ts_now, value_type v, size_type count);

  bool add(time_type ts_now, const data_type& v, size_type count);

  bool add( const reduced_data& v);

  bool push( time_type ts_now, value_type v, size_type count, aggregated_handler handler);

  bool push( time_type ts_now, const data_type& v, size_type count, aggregated_handler handler);

  bool push( const reduced_data& v, aggregated_handler handler);

  aggregated_ptr pop();

  aggregated_ptr force_pop();

  aggregated_ptr aggregate_current();

  bool separate(time_type ts_now, aggregated_handler handler, bool force);

  void enable(bool value);

  simple_pusher_t create_simple_pusher(aggregated_handler handler);

  data_pusher_t create_data_pusher(aggregated_handler handler);

  reduced_pusher_t create_reduced_pusher(aggregated_handler handler);

  template<typename D>
  static time_type now_t()
  {
    return basic_aggregator::now_t<D>();
  }

  size_t size() const;

  void clear(time_type ts_now);

private:
  mutable mutex_type _mutex;
};

}

#pragma once
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/options/aggregator_options.hpp>
#include <wrtstat/aggregator/separator.hpp>
#include <algorithm>

namespace wrtstat {

class basic_aggregator
{
  typedef basic_aggregator self;
public:
  typedef separator::reduced_type reduced_type;
  typedef separator::reduced_ptr reduced_ptr;

  typedef aggregator_options options_type;
  typedef aggregated_data::ptr aggregated_ptr;
  typedef aggregated_data::handler aggregated_handler;
  typedef std::list< aggregated_ptr > aggregated_list;

  basic_aggregator(time_type ts_now, const aggregator_options& opt, const allocator& a = allocator() );

  const separator& get_separator() const;

  bool add(time_type ts_now, value_type v, size_type count);
  bool add(time_type ts_now, const data_type& v, size_type count);
  bool add( const reduced_data& v);

  bool push( time_type ts_now, value_type v, size_type count, aggregated_handler handler);
  bool push( time_type ts_now, const data_type& v, size_type count, aggregated_handler handler);
  bool push( const reduced_data& v, aggregated_handler handler);

  bool separate(bool force = false);
  bool separate(time_type ts_now, aggregated_handler handler, bool force);

  aggregated_ptr pop();

  aggregated_ptr force_pop();

  aggregated_ptr aggregate_current();

  static void calc_perc(aggregated_data& d);

  void enable(bool value);

  size_t size() const;

  size_t size(size_t* data_size, size_t* capacity) const;

  void clear( time_type ts_now );

  template<typename D>
  static time_type now_t()
  {
    return separator::now_t<D>();
  }

  static time_type now(time_type resolution);

  static time_type now(resolutions resolution);

  time_type now();

protected:

  typedef std::function< void(time_type now, time_type value, size_type count) > simple_pusher_t;
  typedef std::function< void(time_type now, data_type&& value, size_type count) > data_pusher_t;
  typedef std::function< void(const reduced_data&) > reduced_pusher_t;

  template<typename T>
  simple_pusher_t create_simple_pusher( std::weak_ptr<T> wthis, aggregated_handler handler );

  template<typename T>
  data_pusher_t create_data_pusher( std::weak_ptr<T> wthis, aggregated_handler handler );

  template<typename T>
  reduced_pusher_t create_reduced_pusher( std::weak_ptr<T> wthis, aggregated_handler handler );

private:

  static value_type nth_(size_type perc, size_type& off, data_type& d);

  size_t reduce_(data_type& d) const;

  void aggregate0_();

  void aggregate1_(aggregated_handler handler);

  static aggregated_ptr aggregate2_(reduced_ptr d);

  void push_handler_( aggregated_ptr ag, aggregated_handler handler);

public:
  separator _sep;
  size_t _outgoing_reduced_size = 0;
  bool _enabled = true;
  aggregated_list _ag_list;
  std::shared_ptr<int> _id;
};

template<typename T>
basic_aggregator::simple_pusher_t basic_aggregator::create_simple_pusher( std::weak_ptr<T> wthis, aggregated_handler handler )
{
  std::weak_ptr<int> wid = this->_id;
  return [wid, wthis, handler](time_type ts_now, time_type v, size_type count)
  {
    if ( auto pthis = wthis.lock() )
    {
      if (auto id = wid.lock() )
      {
        pthis->push(ts_now, v, count, handler);
      }
    }
  };
}

template<typename T>
basic_aggregator::data_pusher_t basic_aggregator::create_data_pusher( std::weak_ptr<T> wthis, aggregated_handler handler )
{
  std::weak_ptr<int> wid = this->_id;
  return [wid, wthis, handler](time_type ts_now, data_type&& v, size_type count)
  {
    if ( auto pthis = wthis.lock() )
    {
      if (auto id = wid.lock() )
      {
        pthis->push(ts_now, std::move(v), count, handler);
      }
    }
  };
}

template<typename T>
basic_aggregator::reduced_pusher_t basic_aggregator::create_reduced_pusher( std::weak_ptr<T> wthis, aggregated_handler handler )
{
  std::weak_ptr<int> wid = this->_id;
  return [wid, wthis, handler]( const reduced_data& reduced)
  {
    if ( auto pthis = wthis.lock() )
    {
      if (auto id = wid.lock() )
      {
        pthis->push(reduced, handler);
      }
    }
  };
}


}

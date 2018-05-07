#pragma once

#include <wrtstat/manager/dict.hpp>
#include <wrtstat/manager/pool.hpp>
#include <wrtstat/manager/mutex/rwlock.hpp>
#include <wrtstat/aggregator.hpp>
#include <wrtstat/types.hpp>

#include <string>
#include <mutex>
#include <deque>
#include <memory>
#include <functional>

namespace wrtstat {

class aggregator_registry
{
public:
  typedef rwlock<std::mutex> mutex_type;
  typedef aggregator_mt aggregator_type;
  typedef typename aggregated_data::ptr aggregated_ptr;
  typedef aggregator_options options_type;
  
  typedef typename aggregator_type::simple_adder_t simple_adder_t;
  typedef typename aggregator_type::data_adder_t data_adder_t;
  typedef typename aggregator_type::reduced_adder_t reduced_adder_t;
  typedef typename aggregator_type::simple_pusher_t simple_pusher_t;
  typedef typename aggregator_type::data_pusher_t data_pusher_t;
  typedef typename aggregator_type::reduced_pusher_t reduced_pusher_t;
  typedef std::function< void(time_type now, time_type value, size_type count, 
                              size_type readed, size_type writed) > composite_adder_t;
  typedef composite_adder_t composite_pusher_t;
  typedef typename aggregator_type::aggregated_handler aggregated_handler;
  
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::deque<aggregator_ptr> aggregator_list;

  explicit aggregator_registry(const options_type& opt, size_t pool_size, id_t init, id_t step);

  size_t aggregators_count() const;

  bool add(id_t id, time_type now, value_type v, size_type count);
  
  bool add( const std::string& name, const reduced_data& v);

  aggregated_ptr force_pop(id_t id);

  aggregated_ptr pop(id_t id);

  std::string get_name(id_t id) const;

  id_t create_aggregator(const std::string& name, time_type now);
  
  aggregator_ptr get_aggregator(const std::string& name, time_type now);
  
  aggregator_ptr get_aggregator(id_t id) const;

  simple_adder_t create_simple_adder( id_t id );

  data_adder_t create_data_adder( id_t id );
  
  reduced_adder_t create_reduced_adder( id_t id );
  
  composite_adder_t create_composite_adder( id_t time_id, id_t read_id, id_t write_id, bool summary_size );

  simple_adder_t create_value_adder(const std::string& name, time_type ts_now);

  data_adder_t create_data_adder(const std::string& name, time_type ts_now);
  
  reduced_adder_t create_reduced_adder( const std::string& name, time_type ts_now );
  
  composite_adder_t create_composite_adder( 
    const std::string& time_name, 
    const std::string& read_name, 
    const std::string& write_name, 
    bool summary_size,
    time_type ts_now );
  
  simple_pusher_t create_simple_pusher( id_t id, aggregated_handler handler );

  data_pusher_t create_data_pusher( id_t id, aggregated_handler handler );
  
  reduced_pusher_t create_reduced_pusher( id_t id, aggregated_handler handler );

  composite_pusher_t create_composite_pusher( 
    id_t time_id, id_t read_id, id_t write_id, 
    aggregated_handler time_handler,
    aggregated_handler read_handler,
    aggregated_handler write_handler,
    bool summary_size
  );

//
  simple_pusher_t create_simple_pusher(const std::string& name, aggregated_handler handler, time_type ts_now);

  data_pusher_t create_data_pusher(const std::string& name, aggregated_handler handler, time_type ts_now);
  
  reduced_pusher_t create_reduced_pusher( const std::string& name, aggregated_handler handler, time_type ts_now );
  
  composite_pusher_t create_composite_pusher( 
    const std::string& time_name, 
    const std::string& read_name, 
    const std::string& write_name, 
    aggregated_handler time_handler,
    aggregated_handler read_handler,
    aggregated_handler write_handler,
    bool summary_size,
    time_type ts_now
  );

  void enable(bool value);

  bool del(const std::string& name);
  
private:
  composite_adder_t make_composite_handler_(simple_adder_t time_adder, simple_adder_t read_adder, simple_adder_t write_adder, bool summary_size  );
private:
  mutable mutex_type _mutex;
  aggregator_options _opt;
  aggregator_list _agarr;
  dict _dict;
  pool<mutex_type> _pool;
  bool _enabled = true;
};

}

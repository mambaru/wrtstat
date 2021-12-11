#pragma once

#include <wrtstat/system/dict.hpp>
#include <wrtstat/system/pool.hpp>
#include <wrtstat/system/rwlock.hpp>
#include <wrtstat/aggregator/aggregator_mt.hpp>
#include <wrtstat/aggregator/options/registry_options.hpp>

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
  typedef registry_options options_type;
  typedef registry_options::initializer_fun_t initializer_fun_t;

  typedef typename aggregator_type::simple_pusher_t simple_pusher_t;
  typedef typename aggregator_type::data_pusher_t data_pusher_t;
  typedef typename aggregator_type::reduced_pusher_t reduced_pusher_t;
  typedef std::function< void(time_type now, time_type value, size_type count,
                              size_type readed, size_type writed) > composite_adder_t;
  typedef composite_adder_t composite_pusher_t;
  typedef typename aggregator_type::aggregated_handler aggregated_handler;

  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::deque<aggregator_ptr> aggregator_list;

  typedef std::pair<std::string, aggregated_ptr> named_aggregated_t;
  typedef std::vector<named_aggregated_t> named_aggregated_list;

  explicit aggregator_registry(const options_type& opt);

  size_t aggregators_count() const;

  bool add(id_t id, time_type now, value_type v, size_type count);

  bool add( const std::string& name, const reduced_data& v);

  aggregated_ptr pop(id_t id) const;

  aggregated_ptr force_pop(id_t id) const;

  void pop_all(named_aggregated_list* ag_list) const;

  void force_pop_all(named_aggregated_list* ag_list) const;

  std::string get_name(id_t id) const;
  
  void set_initializer(initializer_fun_t&& init_f);

  id_t create_aggregator(const std::string& name, time_type now);

  aggregator_ptr get_aggregator(const std::string& name, time_type now);

  aggregator_ptr get_aggregator(id_t id) const;

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
  composite_adder_t make_composite_handler_(
    simple_pusher_t time_adder,
    simple_pusher_t read_adder,
    simple_pusher_t write_adder,
    bool summary_size
  );

  void pop_(named_aggregated_list* ag_list, aggregated_ptr (aggregator_type::*pop_fun)() ) const;
private:
  mutable mutex_type _mutex;
  aggregator_options _opt;
  aggregator_list _agarr;
  dict _dict;
  pool<mutex_type> _pool;
  bool _enabled = true;
  initializer_fun_t initializer_ = nullptr;
};

}

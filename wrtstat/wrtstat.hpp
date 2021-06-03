#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/aggregator/aggregator_registry.hpp>
#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <wrtstat/meters/composite_meter.hpp>
#include <wrtstat/meters/multi_meter.hpp>
#include <mutex>

namespace wrtstat {

// TODO: aggregator_registry as member
class wrtstat
  : public aggregator_registry
{
public:
  typedef aggregator_registry super;
  typedef aggregator_registry manager_type;
  typedef typename manager_type::aggregated_ptr aggregated_ptr;
  typedef std::function<void(const std::string& name, aggregated_data::ptr)> named_aggregated_handler;
  typedef wrtstat_options options_type;

  typedef std::shared_ptr<manager_type> manager_ptr;
  typedef std::mutex mutex_type;

  explicit wrtstat(const options_type& opt = options_type() ) ;

// ----------------------------------
// time_meter
// ----------------------------------

  template<typename D >
  time_meter<D> create_time_meter(id_t id);

  template<typename D >
  time_meter<D> create_time_meter(const std::string& name);

  template<typename D>
  multi_meter< time_meter<D> > create_time_multi_meter( const std::string& time_name);

// ----------------------------------
// size_meter
// ----------------------------------
  size_meter create_size_meter(id_t id);

  size_meter create_size_meter(const std::string& name);

  multi_meter< size_meter > create_size_multi_meter( const std::string& time_name);

// ----------------------------------
// value_meter
// ----------------------------------

  value_meter create_value_meter(id_t id);

  value_meter create_value_meter(const std::string& name);

  multi_meter< value_meter > create_value_multi_meter( const std::string& time_name);

// ----------------------------------
// composite_meter
// ----------------------------------

  template<typename D >
  composite_meter<D> create_composite_meter(id_t time_id, id_t read_id, id_t write_id, bool summary_size);

  template<typename D >
  composite_meter<D> create_composite_meter(
    const std::string& time_name,
    const std::string& read_name,
    const std::string& write_name,
    bool summary_size
  );


  template<typename D>
  multi_meter< composite_meter<D> > create_composite_multi_meter(
    const std::string& time_name,
    const std::string& read_name,
    const std::string& write_name,
    bool summary_size
  );

  template<typename D>
  static time_type now_t();

  time_type now(time_type resolution) const;

  time_type now(resolutions resolution) const;

  time_type now() const;

  size_t pushout() const;

  size_t force_pushout() const;

private:
  
  size_t pushout_(bool force) const;

  static std::string make_name_(const std::string& prefix, const std::string& name);

  aggregated_data::handler make_handler_( id_t id ) const;

  aggregated_data::handler make_handler_( const std::string& name) const;

  void fake_implementations_();
private:
  resolutions _resolution = resolutions::none;
  named_aggregated_handler _handler;
  std::vector<std::string> _prefixes;
};

}

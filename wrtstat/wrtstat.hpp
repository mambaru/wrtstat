#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/meter_manager.hpp>
#include <memory>

namespace wrtstat{

using composite_multi_meter = multi_meter< composite_meter<std::chrono::microseconds> >;
using time_multi_meter = multi_meter< time_meter<std::chrono::microseconds> >;
using value_multi_meter = multi_meter< value_meter >;
using size_multi_meter = multi_meter< size_meter > ;

class wrtstat
  : public meter_manager
{
public:
  typedef wrtstat_options options_type;
  explicit wrtstat(const options_type& opt):meter_manager(opt){}
  /*
  typedef std::function< void(time_type now, time_type value, size_type count) > value_adder_t;
  typedef std::function< void(time_type now, data_type&& value, size_type count) > data_adder_t;
  typedef std::function< void(const reduced_data&) > reduced_adder_t;

  typedef std::shared_ptr<aggregated_data> aggregated_ptr;
  typedef std::chrono::microseconds  duration_type; 
  typedef wrtstat_options options_type;

  virtual ~wrtstat(){}
  wrtstat(options_type opt);
  
  bool add( const std::string& name, const reduced_data& v);
  bool del( const std::string& name );
  
  composite_multi_meter_ptr create_composite_prototype(const std::string& time_name, const std::string& read_name, const std::string& write_name);
  time_multi_meter_ptr create_time_prototype(const std::string& time_name);
  size_multi_meter_ptr create_size_prototype(const std::string& size_name);
  value_multi_meter_ptr create_value_prototype(const std::string& value_name);

  composite_multi_meter_ptr create_meter(composite_multi_meter_ptr m, size_type size );
  time_multi_meter_ptr create_meter(time_multi_meter_ptr m, size_type count );
  size_multi_meter_ptr create_meter(size_multi_meter_ptr m, size_type size );
  value_multi_meter_ptr create_meter(value_multi_meter_ptr m, size_type value, size_type count );
  
  value_adder_t create_value_adder( const std::string& name, time_type ts_now);
  data_adder_t create_data_adder( const std::string& name, time_type ts_now);
  reduced_adder_t create_reduced_adder( const std::string& name, time_type ts_now);
  size_t aggregators_count() const;
  aggregated_ptr pop(id_t id);
  std::string get_name(id_t id) const;
  void enable(bool val);
private:
  std::shared_ptr<impl> _impl;
  time_type _resolution;
  */
};

}

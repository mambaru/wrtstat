#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/manager.hpp>
#include <wrtstat/dict.hpp>
#include <wrtstat/time_meter.hpp>
#include <wrtstat/size_meter.hpp>
#include <wrtstat/pair_meter.hpp>
#include <wrtstat/multi_meter.hpp>
#include <mutex>
#include <chrono>
#include <deque>
#include <memory>

namespace wrtstat {

class wrtstat
{
public:
  typedef manager aggregator_type;
  
  typedef aggregator_type::time_type time_type;
  typedef aggregator_type::value_type value_type;
  typedef aggregator_type::size_type size_type;
  typedef aggregator_type::aggregated_type aggregated_type;
  typedef aggregator_type::aggregated_ptr aggregated_ptr;
  
  typedef wrtstat_options options_type;

  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<std::mutex> mutex_ptr;
  typedef std::deque<mutex_ptr> mutex_list;

  wrtstat(options_type opt = options_type() ) 
    : _prefixes(opt.prefixes)

  { 
    _ag = std::make_shared<aggregator_type>(opt);
    _mutex = std::make_shared<mutex_type>();
    if ( _prefixes.empty() ) 
      _prefixes.push_back("");
  }
  
  void reconfigure(options_type opt)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    _ag = std::make_shared<aggregator_type>(opt);
  }

  bool add(int id, time_type now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->add(id, now, v, count);
  }

  aggregated_ptr pop(int id)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->pop(id);
  }

  aggregated_ptr force_pop(int id)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->force_pop(id);
  }

  int count() const 
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->size();
  }

  int reg_name(std::string name, time_type now)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return this->reg_name_( std::move(name), now);
  }

  std::string get_name(int id) const 
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->get_name(id);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > create_time_meter(int id, time_type now, size_type count)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return this->create_time_meter_<D>(id, now, count);
  }

  std::shared_ptr< size_meter > create_size_meter(int id, time_type now, size_type count, size_type multiple)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return this->create_size_meter_(id, now, count, multiple);
  }
  
  template<typename D >
  std::shared_ptr< pair_meter<D> > create_pair_meter(int rate_id, int size_id, time_type now, size_type count, size_type multiple)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return this->create_pair_meter_<D>(rate_id, size_id, now, count, multiple);
  }

  template<typename D >
  std::shared_ptr< pair_meter<D> > create_pair_meter(std::string time_name, std::string size_name, time_type now, size_type count, size_type multiple)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return this->create_pair_meter_<D>( 
      this->reg_name_( std::move(time_name), now), 
      this->reg_name_( std::move(size_name), now), 
      now, count, multiple);
  }

  template<typename D >
  std::shared_ptr< multi_meter<D> > create_multi_meter(std::string time_name, std::string size_name, time_type now, size_type count, size_type multiple)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return this->create_multi_meter_<D>( std::move(time_name), std::move(size_name), now, count, multiple);
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    _ag->enable(value);
  };
  
private:

  int reg_name_(std::string name, time_type now)
  {
    if ( name.empty() )
      return -1;
    return _ag->reg_name( std::move(name), now);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > create_time_meter_(int id, time_type now, size_type count)
  {
    return std::make_shared< time_meter<D> >(now, count, _ag->create_meter(id), _mutex );
  }

  std::shared_ptr< size_meter > create_size_meter_(int id, time_type now, size_type count, size_type multiple)
  {
    return std::make_shared< size_meter >(now, count, multiple, _ag->create_meter(id), _mutex );
  }

  template<typename D >
  std::shared_ptr< pair_meter<D> > create_pair_meter_(int rate_id, int size_id, time_type now, size_type count, size_type multiple)
  {
    return std::make_shared<pair_meter<D> >( 
      rate_id!=-1 ? this->create_time_meter_< D >(rate_id, now, count) : nullptr,
      size_id!=-1 ? this->create_size_meter_(size_id, now, count, multiple) : nullptr
    );
  }

  template<typename D >
  std::shared_ptr< multi_meter<D> > create_multi_meter_(std::string time_name, std::string size_name, time_type now, size_type size, size_type multiple)
  {
    auto meter = std::make_shared< multi_meter<D> >();
    for ( auto prefix : _prefixes )
    {
      int rate_id = -1;
      int size_id = -1;
      if ( !time_name.empty() )
        rate_id = this->reg_name_(prefix + time_name, now );
      if ( !size_name.empty() )
        size_id = this->reg_name_(prefix + size_name, now );

      meter->push_back( this->create_pair_meter_<D>( rate_id, size_id, now, size, multiple) );
    }
    return meter;
  }

public:
  aggregator_ptr _ag;
  mutex_ptr _mutex;
  std::vector<std::string> _prefixes;

};

}

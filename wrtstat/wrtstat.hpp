#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/manager/manager.hpp>
#include <wrtstat/manager/dict.hpp>
#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/pair_meter.hpp>
#include <wrtstat/meters/multi_meter.hpp>
#include <wrtstat/aggregator.hpp>
#include <mutex>
#include <chrono>
#include <deque>
#include <memory>

namespace wrtstat {

template<typename Manager>
class wrtstat_base
{
public:
  typedef Manager manager_type;
  
  typedef typename manager_type::time_type time_type;
  typedef typename manager_type::value_type value_type;
  typedef typename manager_type::size_type size_type;
  typedef typename manager_type::aggregated_type aggregated_type;
  typedef typename manager_type::aggregated_ptr aggregated_ptr;
  
  typedef wrtstat_options options_type;

  typedef std::shared_ptr<manager_type> manager_ptr;
  typedef std::mutex mutex_type;
  //typedef std::shared_ptr<std::mutex> mutex_ptr;
  //typedef std::deque<mutex_ptr> mutex_list;

  wrtstat_base(options_type opt = options_type() ) 
    : _prefixes(opt.prefixes)

  { 
    _m = std::make_shared<manager_type>(opt);
    //_mutex = std::make_shared<mutex_type>();
    if ( _prefixes.empty() ) 
      _prefixes.push_back("");
  }
  
  /*
  void reconfigure(options_type opt)
  {
   // std::lock_guard<mutex_type> lk( *_mutex);
    _m = std::make_shared<manager_type>(opt);
  }
  */

  bool add(int id, time_type now, value_type v, size_type count)
  {
    //std::lock_guard<mutex_type> lk( *_mutex);
    return _m->add(id, now, v, count);
  }

  aggregated_ptr pop(int id)
  {
    //std::lock_guard<mutex_type> lk( *_mutex);
    return _m->pop(id);
  }

  aggregated_ptr force_pop(int id)
  {
    //std::lock_guard<mutex_type> lk( *_mutex);
    return _m->force_pop(id);
  }

  int count() const 
  {
    //std::lock_guard<mutex_type> lk( *_mutex);
    return _m->size();
  }

  int create_aggregator(std::string name, time_type now)
  {
    //std::lock_guard<mutex_type> lk( *_mutex);
    return this->create_aggregator_( std::move(name), now);
  }

  std::string get_name(int id) const 
  {
    //std::lock_guard<mutex_type> lk( *_mutex);
    return _m->get_name(id);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > create_time_meter(int id, time_type now, size_type count)
  {
    return this->create_time_meter_<D>(id, now, count);
  }

  std::shared_ptr< size_meter > create_size_meter(int id, time_type now, size_type count, size_type multiple)
  {
    return this->create_size_meter_(id, now, count, multiple);
  }
  
  template<typename D >
  std::shared_ptr< pair_meter<D> > create_pair_meter(int rate_id, int size_id, time_type now, size_type count, size_type multiple)
  {
    return this->create_pair_meter_<D>(rate_id, size_id, now, count, multiple);
  }

  template<typename D >
  std::shared_ptr< pair_meter<D> > create_pair_meter( const std::string& time_name, const std::string& size_name, time_type now, size_type count, size_type multiple)
  {
    return this->create_pair_meter_<D>( 
      this->create_aggregator_( time_name, now), 
      this->create_aggregator_( size_name, now), 
      now, count, multiple);
  }

  template<typename D >
  std::shared_ptr< multi_meter<D> > create_multi_meter( const std::string& time_name, const std::string& size_name, time_type now, size_type count, size_type multiple)
  {
    return this->create_multi_meter_<D>( time_name, size_name, now, count, multiple);
  }

  void enable(bool value)
  {
    _m->enable(value);
  };
  
private:

  int create_aggregator_(const std::string& name, time_type now)
  {
    if ( name.empty() )
      return -1;
    return _m->create_aggregator( name, now);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > create_time_meter_(int id, time_type now, size_type count)
  {
    return std::make_shared< time_meter<D> >(now, count, _m->create_meter(id) );
  }

  std::shared_ptr< size_meter > create_size_meter_(int id, time_type now, size_type size, size_type count)
  {
    return std::make_shared< size_meter >(now, size, count, _m->create_meter(id) );
  }

  template<typename D >
  std::shared_ptr< pair_meter<D> > create_pair_meter_(int rate_id, int size_id, time_type now, size_type size, size_type count)
  {
    return std::make_shared<pair_meter<D> >( 
      rate_id!=-1 ? this->create_time_meter_< D >(rate_id, now, count) : nullptr,
      size_id!=-1 ? this->create_size_meter_(size_id, now, size, count) : nullptr
    );
  }

  template<typename D >
  std::shared_ptr< multi_meter<D> > create_multi_meter_(const std::string& time_name, const std::string& size_name, time_type now, size_type size, size_type count)
  {
    auto meter = std::make_shared< multi_meter<D> >();
    for ( auto prefix : _prefixes )
    {
      int rate_id = -1;
      int size_id = -1;
      if ( !time_name.empty() )
        rate_id = this->create_aggregator_(prefix + time_name, now );
      if ( !size_name.empty() )
        size_id = this->create_aggregator_(prefix + size_name, now );

      meter->push_back( this->create_pair_meter_<D>( rate_id, size_id, now, size, count) );
    }
    return meter;
  }

public:
  manager_ptr _m;
  std::vector<std::string> _prefixes;
};

class wrtstat_st: public wrtstat_base<manager_st>
{
public:
  typedef wrtstat_base::options_type options_type;
  wrtstat_st(options_type opt = options_type() ) 
    : wrtstat_base<manager_st>(opt)
  {}
};

class wrtstat_mt: public wrtstat_base<manager_mt>
{
public:
  typedef wrtstat_base::options_type options_type;
  wrtstat_mt(options_type opt = options_type() ) 
    : wrtstat_base<manager_mt>(opt)
  {}
};


}

#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/map_aggregators.hpp>
#include <wrtstat/dict.hpp>
#include <wrtstat/time_meter.hpp>
#include <mutex>
#include <chrono>
#include <deque>
#include <memory>

namespace wrtstat {

class wrtstat
{
public:
  typedef map_aggregators aggregator_type;
  
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
  { 
    _ag = std::make_shared<aggregator_type>(opt);
    _mutex = std::make_shared<mutex_type>();
  }
  
  void reconfigure(options_type opt)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    _ag = std::make_shared<aggregator_type>(opt);
  }

  /*
  bool add(std::string name, time_type now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _ag->add(name, now, v, count);
  }
  */

  bool add(int id, time_type now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    //std::lock_guard<mutex_type> lk2( *this->get_mutex_(id) );
    return _ag->add(id, now, v, count);
  }

  aggregated_ptr pop(int id)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
  //  std::lock_guard<mutex_type> lk2( *this->get_mutex_(id) );
    return _ag->pop(id);
  }

  aggregated_ptr force_pop(int id)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    //std::lock_guard<mutex_type> lk2( *this->get_mutex_(id) );
    return _ag->force_pop(id);
  }

  int count() const 
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->size();
    //return static_cast<int>(_mutex_list.size() );
  }

  int reg_name(const std::string& name, time_type now)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return _ag->reg_name(name, now);
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
    return std::make_shared< time_meter<D> >(now, count, _ag->create_meter(id), _mutex/*this->get_mutex_(id)*/ );
  }

  
  std::shared_ptr< size_meter > create_size_meter(int id, time_type now, size_type count)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    return std::make_shared< size_meter >(now, count, _ag->create_meter(id), _mutex /*this->get_mutex_(id)*/ );
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk( *_mutex);
    _ag->enable(value);
  };
private:
  
  mutex_ptr get_mutex_(int /* id*/ ) const
  {
    return nullptr;
    /*
    if ( id < 0 ) 
      return nullptr;

    if (  static_cast< mutex_list::size_type >(id) >= _mutex_list.size() ) 
      _mutex_list.resize(id+1);
    if ( _mutex_list[id] ==nullptr )
      _mutex_list[id] = std::make_shared<mutex_type>();

    return _mutex_list[id];
    */
  }
  
public:
  aggregator_ptr _ag;
  mutex_ptr _mutex;
  /*mutable mutex_type _mutex;
  mutable mutex_list _mutex_list;
  */
};

}

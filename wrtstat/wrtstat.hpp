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
  }
  
  void reconfigure(options_type opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _ag = std::make_shared<aggregator_type>(opt);
  }

  bool add(std::string name, time_type now, value_type v)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _ag->add(name, now, v);
  }

  bool add(int id, time_type now, value_type v)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _ag->add(id, now, v);
  }

  aggregated_ptr pop(int id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _ag->pop(id);
  }

  aggregated_ptr force_pop(int id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _ag->force_pop(id);
  }

  int count() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return static_cast<int>(_mutex_list.size() );
  }
  int reg_name(const std::string& name, time_type now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _ag->reg_name(name, now);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > create_handler(int id, time_type now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return std::make_shared< time_meter<D> >(now, _ag->create_handler(id), this->get_mutex_(id) );
  }
  
private:
  
  mutex_ptr get_mutex_(int id ) const
  {
    if ( id < 0 ) 
      return nullptr;

    if (  static_cast< mutex_list::size_type >(id) >= _mutex_list.size() ) 
      _mutex_list.resize(id+1);
    if ( _mutex_list[id] ==nullptr )
      _mutex_list[id] = std::make_shared<mutex_type>();

    return _mutex_list[id];
  }
  
public:
  aggregator_ptr _ag;
  mutable mutex_type _mutex;
  mutable mutex_list _mutex_list;
};

}

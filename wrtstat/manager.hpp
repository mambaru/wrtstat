#pragma once

#include <wrtstat/aggregator_options.hpp>
#include <wrtstat/aggregator_map.hpp>
#include <wrtstat/dict.hpp>
#include <mutex>
#include <chrono>
#include <deque>
#include <memory>

namespace wrtstat {

class manager
{
public:
  typedef aggregator_map aggregator_type;
  typedef aggregator_type::time_type time_type;
  typedef aggregator_type::value_type value_type;
  typedef aggregator_type::size_type size_type;
  typedef aggregator_type::aggregated_ptr aggregated_ptr;
  typedef aggregator_type::options_type options_type;

  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<std::mutex> mutex_ptr;
  typedef std::deque<mutex_ptr> mutex_list;

  manager() { 

  }
  
  void reconfigure(options_type opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _ag = std::make_shared<aggregator_type>(opt);
  }

  bool add(std::string name, time_type now, value_type v)
  {
    return _ag->add(name, now, v);
  }

  bool add(int id, time_type now, value_type v)
  {
    return _ag->add(id, now, v);
  }

  aggregated_ptr force_pop(int id)
  {
     return _ag->force_pop(id);
  }


  int reg_name(const std::string& name, time_type now)
  {
    return _ag->reg_name(name, now);
  }

  typedef std::chrono::high_resolution_clock clock_type;
  typedef std::function< void(time_type now, time_type v) > timer_fun_t;

  template<typename D>
  struct time_meter
  {
    typedef D duration_type;

    time_meter(time_type now, timer_fun_t fun, mutex_ptr pmutex)
      : now(now)
      , timer_fun(fun)
      , wmutex(pmutex)
    {
      start = clock_type::now();
    }

    ~time_meter()
    {
      if (timer_fun==nullptr)
        return;
      if ( auto pmutex = wmutex.lock() ) 
      {
        clock_type::time_point finish = clock_type::now();
        time_type span = std::chrono::template duration_cast<D>( finish - start ).count();
        std::lock_guard<mutex_type> lk(*pmutex);
        timer_fun( now, span );
      }
    };

    time_type now;
    timer_fun_t timer_fun;
    std::weak_ptr<mutex_type> wmutex;
    clock_type::time_point start;
  };
  //typedef std::shared_ptr<time_meter> time_meter_ptr;

  template<typename D>
  std::shared_ptr< time_meter<D> > create_handler(int id, time_type now)
  {
    return std::make_shared< time_meter<D> >(now, _ag->create_handler(id), this->get_mutex_(id) );
  }
  
private:
  
  mutex_ptr get_mutex_(int id ) const
  {
    if ( id < 0 ) return nullptr;
    if (  static_cast< mutex_list::size_type >(id) >= _mutex_list.size() ) 
      _mutex_list.resize(id);
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

#pragma once

#include <wrtstat/aggregator_options.hpp>
#include <wrtstat/aggregator_map.hpp>
#include <wrtstat/dict.hpp>
#include <mutex>

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
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::vector<mutex_ptr> mutex_ptr;

  manager() { 

  }
  
  void reconfigure(options_type opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _ag = std::make_shared<aggregator_type>(opt);
    _mutex_list.clear();
  }

  bool add(std::string name, time_type now, value_type v)
  {
    return _ag->add(name, now, v);
  }

  bool add(int id, time_type now, value_type v)
  {
    return _ag->add(id, now, v);
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

    time_meter(time_type now, timer_fun fun)
      : now(now)
      , timer_fun(fun)
    {
      start = clock_type::now();
    }

    ~time_meter()
    {
      if (timer_fun==nullptr)
        return;
      clock_type finish = clock_type::now();
      time_type span = std::chrono::duration_cast<D>( finish - start ).count;
      timer_fun( now, span );
    };

    time_type now;
    timer_fun_t timer_fun;
    clock_type start;
  };
  typedef std::shared_ptr<timer> timer_ptr;

  handler_t create_handler()
  {
    return nullptr;
  }
  
private:
  aggregator_ptr get_(int id)
  {
    if ( id < 0 || static_cast<size_type>(id) >= _agarr.size() )
      return nullptr;
    return _agarr[id];
  }
  
  int findorcre_(std::string name, time_type now)
  {
    int id = _dict.get( std::move(name) );
    if ( id < 0 )
      return id;
    
    if ( _agarr.size() <= static_cast<size_type>(id) )
      _agarr.resize(id + 1);

    if ( _agarr[id] == nullptr )
      _agarr[id] = std::make_shared<aggregator>(now, _opt, _pool.get_allocator() );
    return id;
  }

public:
  aggregator_ptr _ag;
  mutex_list _mutex_list;
  mutable mutex_type _mutex;
};

}

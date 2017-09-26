#pragma once

#include <wrtstat/manager/manager_options.hpp>
#include <wrtstat/manager/dict.hpp>
#include <wrtstat/manager/pool.hpp>
#include <wrtstat/aggregator.hpp>

namespace wrtstat {

template<typename Aggregator>
class manager_base
{
public:
  typedef Aggregator aggregator_type;
  typedef typename aggregator_type::time_type time_type;
  typedef typename aggregator_type::value_type value_type;
  typedef typename aggregator_type::size_type size_type;
  typedef typename aggregator_type::aggregated_type aggregated_type;
  typedef typename aggregator_type::aggregated_ptr aggregated_ptr;
  typedef manager_options options_type;
  typedef typename aggregator_type::meter_fun_t meter_fun_t;
  typedef typename aggregator_type::handler_fun_t handler_fun_t;
  
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::vector<aggregator_ptr> aggregator_list;

  explicit manager_base(const options_type& opt)
    : _opt(opt)
    , _pool(opt.limit, opt.pool)
  { }

  size_t size() const 
  {
    return _agarr.size();
  }

  bool add(int id, time_type now, value_type v, size_type count)
  {
    if ( auto p = this->get_(id) )
      return p->add(now, v, count);
    return false;
  }

  aggregated_ptr force_pop(int id)
  {
    if ( auto p = this->get_(id) )
      return p->force_pop();
    return nullptr;
  }

  aggregated_ptr pop(int id)
  {
    if ( auto p = this->get_(id) )
      return p->pop();
    return nullptr;
  }

  std::string get_name(int id) const 
  {
    return _dict.get_name(id);
  }

  int create_aggregator(const std::string& name, time_type now)
  {
    return this->findorcre_( std::move(name), now );
  }
  
  aggregator_ptr get_aggregator(int id) const
  {
    return this->get_(id);
  }
  
  meter_fun_t create_meter( int id )
  {
    if ( auto ag = this->get_(id) )
      return ag->create_meter();
    return nullptr;
  }

  handler_fun_t create_handler( int id )
  {
    if ( auto ag = this->get_(id) )
      return ag->create_handler();
    return nullptr;
  }

  meter_fun_t create_meter(std::string&& name, time_type ts_now)
  {
    return this->create_meter(
      this->create_aggregator( 
        std::move(name), 
        ts_now
      )
    );
  }

  handler_fun_t create_handler(std::string&& name, time_type ts_now)
  {
    return this->create_handler(
      this->create_aggregator( 
        std::move(name), 
        ts_now)
    );
  }

  void enable(bool value)
  {
    this->_enabled = value;
    for (auto a : _agarr)
      if ( a!=nullptr )
        a->enable(value);
  }
private:

  aggregator_ptr get_(int id) const
  {
    if ( id < 0 || static_cast<size_type>(id) >= _agarr.size() )
      return nullptr;
    return _agarr[ static_cast<size_t>(id) ];
  }
  
  int findorcre_(const std::string& name, time_type now)
  {
    int id = _dict.create_id( name );
    if ( id < 0 )
      return id;
    size_t s_id = static_cast<size_t>(id);
     
    if ( _agarr.size() <= s_id )
      _agarr.resize( s_id + 1 );

    if ( _agarr[s_id] == nullptr )
    {
      _agarr[s_id] = std::make_shared<aggregator_type>(now, _opt, _pool.get_allocator() );
      if ( !_enabled )
        _agarr[s_id]->enable(false);
    }
    return id;
  }

public:
  aggregator_options _opt;
  aggregator_list _agarr;
  dict _dict;
  pool _pool;
  bool _enabled = true;
};

class manager_st: public manager_base<aggregator>
{
  typedef manager_base<aggregator> super;
public:
  typedef super::options_type options_type;
  explicit manager_st( const options_type& opt): manager_base(opt) {};
};

class manager_mt: private manager_base<aggregator_mt>
{
  typedef manager_base<aggregator_mt> super;
public:
  typedef super::aggregator_type aggregator_type;
  typedef super::time_type time_type;
  typedef super::value_type value_type;
  typedef super::size_type size_type;
  typedef super::aggregated_type aggregated_type;
  typedef super::aggregated_ptr aggregated_ptr;
  typedef super::options_type options_type;
  typedef super::meter_fun_t meter_fun_t;
  typedef super::handler_fun_t handler_fun_t;
  typedef super::aggregator_ptr aggregator_ptr;
  typedef std::mutex mutex_type;
  
  explicit manager_mt(const options_type& opt)
    : manager_base(opt) {};
  
  size_t size() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return super::size();
  }

  bool add(int id, time_type now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    return manager_base::add(id, now, v, count);
  }

  aggregated_ptr force_pop(int id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->force_pop();
    return nullptr;
  }

  aggregated_ptr pop(int id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->pop();
    return nullptr;
  }

  std::string get_name(int id) const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return super::get_name(id);
  }

  int create_aggregator(const std::string& name, time_type now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::create_aggregator( name, now );
  }
  
  aggregator_ptr get_aggregator(int id) const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::get_aggregator(id);
  }
  
  meter_fun_t create_meter( int id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_meter();
    return nullptr;
  }
  
  handler_fun_t create_handler(std::string&& name, time_type ts_now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::create_handler( std::move(name), ts_now );
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    manager_base::enable(value);
  }
private:
  mutable mutex_type _mutex;
};

}

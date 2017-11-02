#pragma once

#include <wrtstat/manager/manager_options.hpp>
#include <wrtstat/manager/dict.hpp>
#include <wrtstat/manager/pool.hpp>
#include <mutex>
#include <wrtstat/manager/mutex/empty_mutex.hpp>
#include <wrtstat/manager/mutex/rwlock.hpp>
#include <wrtstat/manager/mutex/spinlock.hpp>
#include <wrtstat/aggregator.hpp>
#include <deque>
#include <array>

namespace wrtstat {

template<typename Aggregator, typename Mutex>
class manager_base
{
public:
  typedef rwlock<Mutex> mutex_type;
  typedef Aggregator aggregator_type;
  typedef typename dict::id_t id_t;
  typedef typename aggregator_type::time_type time_type;
  typedef typename aggregator_type::value_type value_type;
  typedef typename aggregator_type::size_type size_type;
  typedef typename aggregated_data::ptr aggregated_ptr;
  typedef manager_options options_type;
  typedef typename aggregator_type::value_adder_t value_adder_t;
  typedef typename aggregator_type::data_adder_t data_adder_t;
  typedef typename aggregator_type::reduced_adder_t reduced_adder_t;
  
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::deque<aggregator_ptr> aggregator_list;

  explicit manager_base(const options_type& opt, id_t init, id_t step)
    : _opt(opt)
    , _dict(init, step)
    , _pool(opt.reducer_limit, opt.data_pool)
  { }

  size_t size() const 
  {
    read_lock<mutex_type> lk(_mutex);
    return _agarr.size();
  }

  bool add(id_t id, time_type now, value_type v, size_type count)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->add(now, v, count);
    return false;
  }
  
  bool add( const std::string& name, const reduced_data& v)
  {
    if ( auto p = this->create_get_aggregator( name, v.ts ) )
      return p->add(v, nullptr);
    return false;  
  }

  aggregated_ptr force_pop(id_t id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->force_pop();
    return nullptr;
  }

  aggregated_ptr pop(id_t id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->pop();
    return nullptr;
  }

  std::string get_name(id_t id) const 
  {
    read_lock<mutex_type> lk(_mutex);
    return _dict.get_name(id);
  }

  id_t create_aggregator(const std::string& name, time_type now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    id_t id = _dict.create_id( name );
    size_t pos = _dict.id2pos(id);
    if ( pos < _agarr.size() && _agarr[pos]!=nullptr )
      return id;
    
    if ( _agarr.size() <= pos )
      _agarr.resize( pos + 1 );

    if ( _agarr[pos] == nullptr )
    {
      _agarr[pos] = std::make_shared<aggregator_type>(now, _opt, _pool.get_allocator() );
      if ( !_enabled )
        _agarr[pos]->enable(false);
    }
    return id;
  }
  
  aggregator_ptr create_get_aggregator(const std::string& name, time_type now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    id_t id = _dict.create_id( name );
    size_t pos = _dict.id2pos(id);
    if ( pos < _agarr.size() && _agarr[pos]!=nullptr )
      return _agarr[pos];
    
    if ( _agarr.size() <= pos )
      _agarr.resize( pos + 1 );

    if ( _agarr[pos] == nullptr )
    {
      _agarr[pos] = std::make_shared<aggregator_type>(now, _opt, _pool.get_allocator() );
      if ( !_enabled )
        _agarr[pos]->enable(false);
    }
    return _agarr[pos];
  }
  
  aggregator_ptr get_aggregator(id_t id) const
  {
    read_lock<mutex_type> lk(_mutex);
    size_t pos = _dict.id2pos(id);
    if ( pos >= _agarr.size() )
      return nullptr;
    return _agarr[ pos ];
  }

  value_adder_t create_value_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_value_adder();
    return nullptr;
  }

  data_adder_t create_data_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_data_adder();
    return nullptr;
  }
  
  reduced_adder_t create_reduced_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_reduced_adder();
    return nullptr;
  }

  value_adder_t create_value_adder(const std::string& name, time_type ts_now)
  {
    return this->create_value_adder(
      this->create_aggregator( 
        name, 
        ts_now
      )
    );
  }

  data_adder_t create_data_adder(const std::string& name, time_type ts_now)
  {
    return this->create_data_adder(
      this->create_aggregator( 
        name, 
        ts_now)
    );
  }
  
  reduced_adder_t create_reduced_adder( const std::string& name, time_type ts_now )
  {
    return this->create_reduced_adder(
      this->create_aggregator( 
        name, 
        ts_now)
    );
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    this->_enabled = value;
    for (auto a : _agarr)
    {
      if ( a!=nullptr )
        a->enable(value);
    }
  }

  bool del(const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);

    id_t id = this->_dict.get_id(name);
    if ( id == static_cast<id_t>(-1) )
      return false;
    size_t pos = _dict.id2pos(id);
    _agarr[ pos ] = nullptr;
    return this->_dict.free(id);
  }
  
private:
  mutable mutex_type _mutex;
  aggregator_options _opt;
  aggregator_list _agarr;
  dict _dict;
  pool _pool;
  bool _enabled = true;
};

class manager_st: public manager_base<aggregator, empty_mutex>
{
  typedef manager_base<aggregator, empty_mutex> super;
public:
  typedef super::options_type options_type;
  explicit manager_st( const options_type& opt, id_t init, id_t step): manager_base(opt, init, step) {};
};

class manager_mt: public manager_base<aggregator_mt, std::mutex>
{
  typedef manager_base<aggregator_mt, std::mutex> super;
public:
  typedef super::options_type options_type;
  explicit manager_mt( const options_type& opt, id_t init, id_t step): manager_base(opt, init, step) {};
};

/*
class manager_hash
{
  static constexpr std::size_t hash_size = 1024;
  typedef manager_mt super;
public:
  typedef std::shared_ptr<manager_mt> manager_ptr;
  typedef std::vector<manager_ptr> manager_list;
  typedef super::aggregator_type aggregator_type;
  typedef super::time_type time_type;
  typedef super::value_type value_type;
  typedef super::size_type size_type;
  typedef super::aggregated_type aggregated_type;
  typedef super::aggregated_ptr aggregated_ptr;
  typedef super::options_type options_type;
  typedef super::meter_fun_t meter_fun_t;
  typedef super::handler_fun_t handler_fun_t;
  typedef super::aggregator_fun_t aggregator_fun_t;
  typedef super::aggregator_ptr aggregator_ptr;
  typedef std::mutex mutex_type;
  
  explicit manager_hash(const options_type& opt)
    //: manager_mt(opt)
  {
    _managers.resize(hash_size);
    for (auto& pm : _managers )
      pm = std::make_shared<manager_mt>(opt);
  };
  
  
  manager_ptr by_name( const std::string& name )
  {
    size_t pos = std::hash<std::string>(name) % hash_size;
    return _managers[pos];
  }
  
  manager_ptr by_name( int id )
  {
    
  }
  
  
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
  
  handler_fun_t create_handler( const std::string& name, time_type ts_now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::create_handler( name, ts_now );
  }

  aggregator_fun_t create_aggregator_handler( const std::string& name, time_type ts_now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::create_aggregator_handler( name, ts_now );
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    manager_base::enable(value);
  }

  bool del(const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::del(name);
  }
private:
  mutable mutex_type _mutex;
  manager_list _managers;
};
*/

/*
class manager_mt: private manager_base<aggregator_mt, std::mutex>
{
  typedef manager_base<aggregator_mt, std::mutex> super;
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
  typedef super::aggregator_fun_t aggregator_fun_t;
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
  
  handler_fun_t create_handler( const std::string& name, time_type ts_now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::create_handler( name, ts_now );
  }

  aggregator_fun_t create_aggregator_handler( const std::string& name, time_type ts_now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::create_aggregator_handler( name, ts_now );
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    manager_base::enable(value);
  }

  bool del(const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return manager_base::del(name);
  }
private:
  mutable mutex_type _mutex;
};
*/
}

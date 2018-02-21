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
    {
      std::cout << "get_aggregator" << std::endl;
      abort();
      return nullptr;
    }
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

}

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
  typedef typename aggregator_type::set_span_fun_t set_span_fun_t;
  
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::vector<aggregator_ptr> aggregator_list;

  manager_base(options_type opt)
    : _opt(opt)
    , _pool(opt.limit, 100000 /*todo*/)
  { }

  size_t size() const 
  {
    return _agarr.size();
  }

  bool add(const std::string& name, time_type now, value_type v, size_type count)
  {
    int id = this->findorcre_(std::move(name), now);
    return this->add(id , now, v, count );
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
  
  set_span_fun_t create_meter( int id )
  {
    if ( auto ag = this->get_(id) )
      return ag->create_meter();
    return nullptr;
  }

  void enable(bool value)
  {
    _enabled = value;
    for (auto a : _agarr)
      if ( a!=nullptr )
        a->enable(value);
  }
private:

  aggregator_ptr get_(int id) const
  {
    if ( id < 0 || static_cast<size_type>(id) >= _agarr.size() )
      return nullptr;
    return _agarr[id];
  }
  
  int findorcre_(const std::string& name, time_type now)
  {
    int id = _dict.create_id( name );
    if ( id < 0 )
      return id;
    
    if ( _agarr.size() <= static_cast<size_type>(id) )
      _agarr.resize(id + 1);

    if ( _agarr[id] == nullptr )
    {
      _agarr[id] = std::make_shared<aggregator_type>(now, _opt, _pool.get_allocator() );
      if ( !_enabled )
        _agarr[id]->enable(false);
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
  manager_st(options_type opt): manager_base(opt) {};
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
  typedef super::set_span_fun_t set_span_fun_t;
  typedef super::aggregator_ptr aggregator_ptr;
  typedef std::mutex mutex_type;
  
  manager_mt(options_type opt)
    : manager_base(opt) {};
  
  size_t size() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return super::size();
  }

  bool add(const std::string& name, time_type now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return super::add(name, now, v, count);
  }

  bool add(int id, time_type now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return super::add(id, now, v, count);
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
    return super::create_aggregator( name, now );
  }
  
  aggregator_ptr get_aggregator(int id) const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return super::get_aggregator(id);
  }
  
  set_span_fun_t create_meter( int id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_meter();
    return nullptr;
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    super::enable(value);
  }
private:
  mutable mutex_type _mutex;
};

}

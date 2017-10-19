#pragma once
#include <wrtstat/aggregator.hpp>
#include <wrtstat/allocator.hpp>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace wrtstat {

struct aggregator_map_options: aggregator_base::options_type
{
  time_t random_startup_offset = 0;
};

class aggregator_map
{
public:
  typedef aggregator_base aggregator_type;
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef aggregator_map_options options_type;
  typedef aggregator_base::pop_handler_fun_t pop_handler_fun_t;
  typedef aggregator_base::aggregated_ptr aggregated_ptr;
  typedef std::unordered_map<std::string, aggregator_ptr> aggregator_map_t;
  
  aggregator_map(const options_type& opt = options_type(), const allocator& a = allocator() )
    : _opt(opt)
    , _allocator(a)
  {
  }

  bool add( const std::string& name, const reduced_data& v, pop_handler_fun_t handler)
  {
    auto itr = _aggregator_map.find(name);
    if ( itr == _aggregator_map.end() )
    {
      time_t create_ts = v.ts - ( _opt.random_startup_offset ? rand()*rand() % _opt.random_startup_offset : 0 );
      itr = _aggregator_map.insert( 
        std::make_pair(
          name, 
          std::make_shared<aggregator_type>(
            create_ts, 
            _opt, 
            _allocator
          ) 
        )  
      ).first;
    }
    
    return itr->second->add(v, std::move(handler) );
  }
  
private:
  aggregator_map_t _aggregator_map;
  options_type _opt;
  allocator _allocator;
};

struct aggregator_hash_options: aggregator_map::options_type
{
  size_t hash_size = 4096;
};

class aggregator_hash
{
  typedef std::mutex mutex_type;
public:
  typedef aggregator_map::aggregated_ptr aggregated_ptr;
  typedef aggregator_map::pop_handler_fun_t pop_handler_fun_t;
  typedef aggregator_hash_options options_type;

  aggregator_hash(const options_type& opt = options_type() )
    :_opt(opt)
  {
    this->initialize_();
  }
  
  aggregator_hash(aggregator_hash&& agh) 
    :_opt(agh._opt)
    ,_mutex_list( std::move(agh._mutex_list) )
    ,_aggregator_list( std::move(agh._aggregator_list) )
  {
  }
  
  aggregator_hash(const aggregator_hash& agh)
    :_opt(agh._opt)
  {
    this->initialize_();
  };

  aggregator_hash& operator=(aggregator_hash&& agh)
  {
    _opt = agh._opt;
    _mutex_list = std::move(agh._mutex_list);
    _aggregator_list = std::move(agh._aggregator_list);
    return *this;
  };
  
  bool add( const std::string& name, const reduced_data& v, pop_handler_fun_t handler)
  {
    size_t pos = this->get_pos_(name);
    auto& ag = _aggregator_list[pos];
    auto& mtx = _mutex_list[pos];
    std::lock_guard<mutex_type> lk( *mtx );
    return ag->add(name, v, std::move(handler) );
  }
  
private:
  size_t get_pos_( const std::string& name) const
  {
    return _hash(name) % _opt.hash_size;
  }
  
  void initialize_()
  {
    _mutex_list.resize( _opt.hash_size);
    _aggregator_list.resize( _opt.hash_size);
    for (auto& m : _mutex_list) m = mutex_ptr( new mutex_type() );
    for (auto& m : _aggregator_list) m = aggregator_ptr(new aggregator_map(_opt));
  }
  
private:
  std::hash<std::string> _hash;
  options_type _opt;
  //size_t _hash_size;
  typedef std::unique_ptr<mutex_type> mutex_ptr;
  typedef std::unique_ptr<aggregator_map> aggregator_ptr;
  std::vector<mutex_ptr> _mutex_list;
  std::vector<aggregator_ptr> _aggregator_list;
};


}

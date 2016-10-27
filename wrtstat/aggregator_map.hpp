#pragma once

#include <wrtstat/aggregator_options.hpp>
#include <wrtstat/aggregator.hpp>
#include <wrtstat/dict.hpp>

namespace wrtstat {

class aggregator_map
{
public:
  typedef aggregator aggregator_type;
  typedef aggregator_type::time_type time_type;
  typedef aggregator_type::value_type value_type;
  typedef aggregator_type::size_type size_type;
  typedef aggregator_type::aggregated_ptr aggregated_ptr;
  
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::vector<aggregator_ptr> aggregator_list;

  aggregator_map(aggregator_options opt)
    : _opt(opt)
    , _pool(opt.limit, 100000 /*todo*/)
  { }

  bool add(std::string name, time_type now, value_type v)
  {
    int id = this->findorcre_(std::move(name), now);
    return this->add(id , now, v );
  }

  bool add(int id, time_type now, value_type v)
  {
    if ( auto p = this->get_(id) )
      return p->add(now, v);
    return false;
  }
  
  aggregated_ptr pop()
  {
    return nullptr;
    /*
    if ( _ag_list.empty() ) 
      return nullptr;
    auto res = std::move(_ag_list.front() );
    _ag_list.pop_front();
    return res;
    */
  }
  
  aggregated_ptr add_and_pop(time_type /*now*/, value_type /*v*/)
  {
    return nullptr;
    /*
    return this->aggregate_(_sep.add_and_pop(now, v));
    */
  }

  int reg_name(const std::string& name, time_type now)
  {
    return this->findorcre_(name, now);
  }

  int create_aggregator(std::string name, time_type now)
  {
    return this->findorcre_( std::move(name), now );
  }
  
  aggregator_ptr get_aggregator(int id)
  {
    return this->get_(id);
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
  aggregator_options _opt;
  aggregator_list _agarr;
  dict _dict;
  pool _pool;
};

}

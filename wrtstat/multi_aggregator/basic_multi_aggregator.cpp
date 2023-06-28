//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#include "basic_multi_aggregator.hpp"
#include <fas/system/memory.hpp>

namespace wrtstat {

basic_multi_aggregator::basic_multi_aggregator(const options_type& opt )
  : _opt(opt)
  , _pool(opt.reducer_limit, opt.pool_size)
{
}

bool basic_multi_aggregator::push( const std::string& name, const reduced_data& v, const push_handler& handler)
{
  auto itr = _aggregator_map.find(name);
  if ( itr == _aggregator_map.end() )
  {
    itr = _aggregator_map.insert( 
      std::make_pair(
        name, 
        std::make_shared<aggregator_type>(
          v.ts, 
          _opt, 
          _pool.get_allocator()
        ) 
      )  
    ).first;
  }
  return itr->second->push(v, [handler, &name](aggregated_data::ptr ag){
    auto p = std::make_unique<request::push>();
    p->name = name;
    static_cast<reduced_data&>(*p) = std::move(*ag);
    handler(std::move(p));
  } );
}

bool basic_multi_aggregator::del( const std::string& name)
{
  auto itr = _aggregator_map.find(name);
  if ( itr == _aggregator_map.end() )
    return false;
  _aggregator_map.erase(itr);
  return true;
}


bool basic_multi_aggregator::push( const request::push& p, const push_handler& handler)
{
  return this->push(p.name, static_cast<const reduced_data&>(p), handler );
}

void basic_multi_aggregator::pushout(const push_handler& handler)
{
  for ( auto& item : _aggregator_map )
  {
    if ( aggregated_data::ptr ag = item.second->pop() )
    {
      auto p = std::make_unique<request::push>();
      p->name = item.first;
      static_cast<aggregated_data&>(*p) = std::move(*ag);
      handler( std::move(p) );
    }
  }
}

void basic_multi_aggregator::force_pushout(const push_handler& handler)
{
  for ( auto& item : _aggregator_map )
  {
    if ( aggregated_data::ptr ag = item.second->force_pop() )
    {
      auto p = std::make_unique<request::push>();
      p->name = item.first;
      static_cast<aggregated_data&>(*p) = std::move(*ag);
      handler( std::move(p) );
    }
  }
}


}

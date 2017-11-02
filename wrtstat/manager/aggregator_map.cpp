#include <wrtstat/manager/aggregator_map.hpp>

namespace wrtstat {

aggregator_map::aggregator_map(const options_type& opt )
  : _opt(opt)
  , _pool(opt.limit, opt.pool_size)
{
}

bool aggregator_map::add( const std::string& name, const reduced_data& v, aggregated_handler handler)
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
  return itr->second->add(v, std::move(handler) );
}

}

#include <wrtstat/manager/aggregator_hashmap.hpp>

namespace wrtstat {

aggregator_hashmap::aggregator_hashmap(const options_type& opt, size_t hash_size, size_t pool_size )
  : _opt(opt)
  , _hash_size(hash_size)
{
  this->initialize_(hash_size, pool_size);
}
  
aggregator_hashmap::aggregator_hashmap(aggregator_hashmap&& agh) 
  : _hash( std::move(agh._hash) )
  , _opt( std::move(agh._opt) )
  , _hash_size(agh._hash_size)
  ,_mutex_list( std::move(agh._mutex_list) )
  ,_aggregator_list( std::move(agh._aggregator_list) )
{
}
  
  /*
aggregator_hashmap_mt::aggregator_hashmap_mt(const aggregator_hashmap_mt& agh)
  : _hash(agh._hash)
  , _opt(agh._opt)
  , _hash_size(agh._hash_size)
{
  this->initialize_();
}*/

aggregator_hashmap& aggregator_hashmap::operator=(aggregator_hashmap&& agh)
{
  _opt = agh._opt;
  _mutex_list = std::move(agh._mutex_list);
  _hash = agh._hash;
  _hash_size = agh._hash_size;
  _aggregator_list = std::move(agh._aggregator_list);
  return *this;
}
  
bool aggregator_hashmap::add( const std::string& name, const reduced_data& v, aggregated_handler handler)
{
  size_t pos = this->get_pos_(name);
  auto& ag = _aggregator_list[pos];
  auto& mtx = _mutex_list[pos];
  std::lock_guard<mutex_type> lk( *mtx );
  return ag->add(name, v, std::move(handler) );
}
  
size_t aggregator_hashmap::get_pos_( const std::string& name) const
{
  return _hash(name) % _hash_size;
}
  
void aggregator_hashmap::initialize_(size_t hash_size, size_t pool_size)
{
  _mutex_list.resize( hash_size);
  _aggregator_list.resize( hash_size);
  for (auto& m : _mutex_list) m = mutex_ptr( new mutex_type() );
  for (auto& m : _aggregator_list) m = aggregator_ptr(new aggregator_map(_opt, pool_size));
}

}

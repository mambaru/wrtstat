#include <wrtstat/manager/aggregator_hashmap.hpp>

namespace wrtstat {

aggregator_hashmap_mt::aggregator_hashmap_mt(const options_type& opt )
  :_opt(opt)
{
  this->initialize_();
}
  
aggregator_hashmap_mt::aggregator_hashmap_mt(aggregator_hashmap_mt&& agh) 
  :_opt(agh._opt)
  ,_mutex_list( std::move(agh._mutex_list) )
  ,_aggregator_list( std::move(agh._aggregator_list) )
{
}
  
aggregator_hashmap_mt::aggregator_hashmap_mt(const aggregator_hashmap_mt& agh)
  :_opt(agh._opt)
{
  this->initialize_();
}

aggregator_hashmap_mt& aggregator_hashmap_mt::operator=(aggregator_hashmap_mt&& agh)
{
  _opt = agh._opt;
  _mutex_list = std::move(agh._mutex_list);
  _aggregator_list = std::move(agh._aggregator_list);
  return *this;
}
  
bool aggregator_hashmap_mt::add( const std::string& name, const reduced_data& v, aggregated_handler handler)
{
  size_t pos = this->get_pos_(name);
  auto& ag = _aggregator_list[pos];
  auto& mtx = _mutex_list[pos];
  std::lock_guard<mutex_type> lk( *mtx );
  return ag->add(name, v, std::move(handler) );
}
  
size_t aggregator_hashmap_mt::get_pos_( const std::string& name) const
{
  return _hash(name) % _opt.hash_size;
}
  
void aggregator_hashmap_mt::initialize_()
{
  _mutex_list.resize( _opt.hash_size);
  _aggregator_list.resize( _opt.hash_size);
  for (auto& m : _mutex_list) m = mutex_ptr( new mutex_type() );
  for (auto& m : _aggregator_list) m = aggregator_ptr(new aggregator_map(_opt));
}

}

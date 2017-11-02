#pragma once
#include <wrtstat/manager/aggregator_map.hpp>
#include <wrtstat/manager/aggregator_hashmap_options.hpp>


namespace wrtstat {

class aggregator_hashmap_mt
{
  typedef std::mutex mutex_type;
public:
  typedef aggregator_map::aggregated_ptr aggregated_ptr;
  typedef aggregator_map::aggregated_handler aggregated_handler;
  typedef aggregator_hashmap_options options_type;

  aggregator_hashmap_mt(const options_type& opt = options_type() );
  
  aggregator_hashmap_mt(aggregator_hashmap_mt&& agh);
  
  aggregator_hashmap_mt(const aggregator_hashmap_mt& agh);

  aggregator_hashmap_mt& operator=(aggregator_hashmap_mt&& agh);
  
  bool add( const std::string& name, const reduced_data& v, aggregated_handler handler);
  
private:
  size_t get_pos_( const std::string& name) const;
  
  void initialize_();
  
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

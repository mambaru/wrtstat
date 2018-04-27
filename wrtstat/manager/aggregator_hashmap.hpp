#pragma once
#include <wrtstat/manager/aggregator_map.hpp>
#include <wrtstat/manager/aggregator_hashmap_options.hpp>


namespace wrtstat {

class aggregator_hashmap
{
  typedef std::mutex mutex_type;
public:
  typedef aggregator_map::aggregated_ptr aggregated_ptr;
  typedef aggregator_map::aggregated_handler aggregated_handler;
  typedef aggregator_options options_type;

  explicit aggregator_hashmap(const options_type& opt = options_type(), size_t hash_size = 4096, size_t pool_size = 0 );
  
  aggregator_hashmap(aggregator_hashmap&& agh);
  
  aggregator_hashmap(const aggregator_hashmap& agh) = delete;

  aggregator_hashmap& operator=(aggregator_hashmap&& agh);
  
  bool add( const std::string& name, const reduced_data& v, aggregated_handler handler);
  
private:
  size_t get_pos_( const std::string& name) const;
  
  void initialize_(size_t hash_size, size_t pool_size);
  
private:
  std::hash<std::string> _hash;
  options_type _opt;
  size_t _hash_size;
  typedef std::unique_ptr<mutex_type> mutex_ptr;
  typedef std::unique_ptr<aggregator_map> aggregator_ptr;
  std::vector<mutex_ptr> _mutex_list;
  std::vector<aggregator_ptr> _aggregator_list;
};


}

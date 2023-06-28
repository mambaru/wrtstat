//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wrtstat/aggregator/basic_aggregator.hpp>
#include <wrtstat/system/pool.hpp>
#include <wrtstat/system/empty_mutex.hpp>
#include <wrtstat/multi_aggregator/multi_aggregator_options.hpp>
#include <wrtstat/api/push.hpp>
#include <unordered_map>

namespace wrtstat {

class basic_multi_aggregator
{
public:
  typedef basic_aggregator aggregator_type;
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef multi_aggregator_options options_type;
  //typedef basic_aggregator::aggregated_handler aggregated_handler;
  //typedef basic_aggregator::aggregated_ptr aggregated_ptr;
  typedef std::function<void(request::push::ptr)> push_handler;
  typedef std::unordered_map<std::string, aggregator_ptr> aggregator_map_t;
  
  explicit basic_multi_aggregator(const options_type& opt = options_type() );

  bool push( const std::string& name, const reduced_data& v, const push_handler& handler);

  bool push( const request::push& p, const push_handler& handler);
  
  bool del( const std::string& name);

  void pushout(const push_handler& handler);
  void force_pushout(const push_handler& handler);
private:
  aggregator_map_t _aggregator_map;
  options_type _opt;
  pool<empty_mutex> _pool;
};

}

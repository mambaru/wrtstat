//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wrtstat/multi_aggregator/basic_multi_aggregator.hpp>
#include <wrtstat/multi_aggregator/multi_aggregator_options.hpp>
#include <wrtstat/api/multi_push.hpp>

namespace wrtstat{

class multi_aggregator
{
  typedef std::mutex mutex_type;
public:
  // typedef basic_multi_aggregator::aggregated_ptr aggregated_ptr;
  // typedef basic_multi_aggregator::aggregated_handler aggregated_handler;
  typedef multi_aggregator_options options_type;
  typedef std::function<void(request::push::ptr)> push_handler;
  
  explicit multi_aggregator(const options_type& opt = options_type());
  
  multi_aggregator(multi_aggregator&& agh);
  
  multi_aggregator(const multi_aggregator& agh) = delete;

  multi_aggregator& operator=(multi_aggregator&& agh);
  
  bool push( const std::string& name, const reduced_data& v, push_handler handler);
  
  bool push( const request::push& p, const push_handler& handler);

  bool multi_push( const request::multi_push& p, const push_handler& handler);
  
  bool del( const std::string& name);

  void pushout(const push_handler& handler);
  void force_pushout(const push_handler& handler);

private:
  size_t get_pos_( const std::string& name) const;
  
  void initialize_();
  
private:
  std::hash<std::string> _hash;
  options_type _opt;
  typedef std::unique_ptr<mutex_type> mutex_ptr;
  typedef std::unique_ptr<basic_multi_aggregator> aggregator_ptr;
  std::vector<mutex_ptr> _mutex_list;
  std::vector<aggregator_ptr> _aggregator_list;
};


}

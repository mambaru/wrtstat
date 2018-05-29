#pragma once
#include <wrtstat/reducer.hpp>
#include <wrtstat/separator_options.hpp>
#include <list>
#include <ctime>
#include <iostream>

namespace wrtstat {

class separator
{
public:
  typedef reducer reducer_type;
  typedef reducer_type::reduced_type reduced_type;
  typedef reducer_type::reduced_ptr  reduced_ptr; 
  typedef aggregated_data::handler aggregated_handler;
  
  separator( time_type ts_now, const separator_options& opt, const allocator& a = allocator() );

  const reducer_type& get_reducer() const;

  bool add( time_type ts, value_type v, size_type count );
  
  bool add( time_type ts, const data_type& v, size_type count );
  
  bool add( time_type ts, std::initializer_list<value_type> v );
  
  bool add( const reduced_data& v );

  bool push( time_type ts, value_type v, size_type count, aggregated_handler handler );

  bool push( time_type ts, const data_type& v, size_type count, aggregated_handler handler );

  bool push( const reduced_data& v, aggregated_handler handler );

  reduced_ptr pop();

  reduced_ptr force_pop();
  
  reduced_ptr get_current();
  
  time_type next_time() const;

  time_type get_ts(time_type ts) const;
  
  //bool separate(time_type ts_now, aggregated_handler handler);
  
  bool separate(time_type ts_now, aggregated_handler handler, bool force);
  
  bool ready() const;

  bool empty() const;
  
  template<typename D>
  static time_type now_t() 
  {
    static std::chrono::time_point< std::chrono::system_clock, D > beg;
    return std::chrono::duration_cast<D>(std::chrono::system_clock::now() - beg ).count();
  }
  
  static time_type now(time_type resolution);
  
  static time_type now(resolutions resolution);
  
  time_type now();

  size_t size() const;

  void clear( time_type ts_now );

  time_type current_time() const;  
  
private:
  
  reducer_type _reducer;
  const time_type _step_ts;
  time_type _next_time;
  resolutions _resolution;
  std::list<reduced_ptr> _sep_list;
};


}

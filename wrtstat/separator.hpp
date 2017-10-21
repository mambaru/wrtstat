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
  typedef types::time_type time_type;
  typedef reducer reducer_type;
  typedef reducer_type::size_type   size_type;

  typedef reducer_type::data_type    data_type;
  typedef reducer_type::value_type   value_type;
  typedef reducer_type::reduced_type reduced_type;
  typedef reducer_type::reduced_ptr  reduced_ptr; 
  typedef aggregated_data aggregated_type;
  typedef std::unique_ptr<aggregated_type> aggregated_ptr;
  typedef std::function< void(aggregated_ptr) > pop_handler_fun_t;


  separator( time_type ts_now, const separator_options& opt, const allocator& a = allocator() )
    : _reducer(opt, a)
    , _step_ts(opt.step_ts)
    , _next_time(0)
    , _resolution(opt.resolution)
  {
    ts_now = this->get_ts(ts_now);
    if ( opt.soiled_start!=0 )
    {
      size_t rnd = static_cast<size_t>(std::rand());
      //std::cout << ts_now << " " << (soiled*soiled) % size_t(opt.soiled_start) << std::endl;
      ts_now -= time_type(rnd*rnd) % opt.soiled_start;
    }
    _next_time = ts_now + opt.step_ts;
  }

  const reducer_type& get_reducer() const 
  {
    return this->_reducer;
  }

  bool add( time_type ts_now, value_type v, size_type count )
  {
    time_type ts = this->get_ts(ts_now);
    this->separate(ts, false);
    if ( ts < _next_time - _step_ts )
      return false;
    _reducer.add(v, count);
    return true;
  }
  
  bool add( time_type ts_now, const data_type& v, size_type count )
  {
    time_type ts = this->get_ts(ts_now);
    this->separate(ts, false);
    if ( ts < _next_time - _step_ts )
      return false;
    _reducer.add(v, count);
    return true;
  }
  
  bool add( time_type ts_now, std::initializer_list<value_type> v )
  {
    time_type ts = this->get_ts(ts_now);
    this->separate(ts, false);
    if ( ts < _next_time - _step_ts )
      return false;
    _reducer.add( std::move(v) );
    return true;
  }
  
  bool add( const reduced_data& v )
  {
    time_type ts = this->get_ts(v.ts);
    this->separate( ts, false);
    if ( ts < _next_time - _step_ts )
      return false;
    _reducer.add( v );
    return true;
  }

  bool add( const reduced_data& v, pop_handler_fun_t handler )
  {
    time_type ts = this->get_ts(v.ts);
    this->separate( ts, handler);
    if ( ts < _next_time - _step_ts )
      return false;
    _reducer.add( v );
    return true;
  }

  reduced_ptr pop()
  {
    if ( _sep_list.empty() )
      return nullptr;
    auto res = std::move(_sep_list.front());
    _sep_list.pop_front();
    return res;
  }

  reduced_ptr force_pop()
  {
    if (auto r = this->pop() )
      return r; 
    auto r = _reducer.detach();
    r->ts = _next_time;
    return r;
  }

  time_type next_time() const 
  {
    return _next_time;
  }

  time_type current_time() const 
  {
    return _next_time - _step_ts;
  }

  time_type get_ts(time_type ts)
  {
    return ts!=0 ? ts : separator::now(_resolution);
  }
  
  bool separate(time_type ts_now, pop_handler_fun_t handler)
  {
    if ( ts_now < _next_time )
      return false;

    while ( _next_time <= ts_now )
    {
      if ( auto res = _reducer.detach() )
      {
        res->ts = _next_time - _step_ts;
        if ( handler!=nullptr) 
          handler( std::move(res) );
      }
      _next_time += _step_ts;
    }
    return true;
  }
  
  bool separate(time_type ts_now, bool force)
  {
    if ( !force && ts_now < _next_time )
      return false;

    while ( _next_time <= ts_now )
    {
      if ( auto res = _reducer.detach() )
      {
        res->ts = _next_time - _step_ts;
        _sep_list.push_back( std::move(res) );
      }
      _next_time += _step_ts;
    }
    return true;
  }
  
  bool ready() const
  {
    return !_sep_list.empty();
  }

  bool empty() const
  {
    return _sep_list.empty();
  }
  
  template<typename D>
  static time_type now() 
  {
    static std::chrono::time_point< std::chrono::system_clock, D > beg;
    return std::chrono::duration_cast<D>(std::chrono::system_clock::now() - beg ).count();
  }
  
  static time_type now(time_type resolution)
  {
    switch (resolution)
    {
      case 1 : return separator::now<std::chrono::seconds>();
      case 1000 : return separator::now<std::chrono::milliseconds>();
      case 1000000 : return separator::now<std::chrono::microseconds>();
      case 1000000000 : return separator::now<std::chrono::nanoseconds>();
      default:
        return 0;
    };
    return 0;
  }

  size_t size() const 
  {
    return _sep_list.size();
  }

  void clear( time_type ts_now )
  {
    _next_time = ts_now + _step_ts;
    _reducer.clear();
    _sep_list.clear();
  }
  
private:
  reducer_type _reducer;
  const time_type _step_ts;
  time_type _next_time;
  time_type _resolution;
  std::list<reduced_ptr> _sep_list;
};


}

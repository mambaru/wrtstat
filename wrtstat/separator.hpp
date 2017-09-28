#pragma once
#include <wrtstat/reducer.hpp>
#include <wrtstat/separator_options.hpp>
#include <list>
#include <ctime>

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

  separator( time_type ts_now, const separator_options& opt, const allocator& a = allocator() )
    : _reducer(opt, a)
    , _step_ts(opt.step_ts)
    , _next_time(ts_now + opt.step_ts)
  {
  }

  const reducer_type& get_reducer() const 
  {
    return this->_reducer;
  }

  bool add( time_type ts_now, value_type v, size_type count )
  {
    this->separate(ts_now, false);
    if ( ts_now < _next_time - _step_ts )
      return false;
    _reducer.add(v, count);
    return true;
  }
  
  bool add( time_type ts_now, const data_type& v, size_type count )
  {
    this->separate(ts_now, false);
    if ( ts_now < _next_time - _step_ts )
      return false;
    _reducer.add(v, count);
    return true;
  }
  
  bool add( time_type ts_now, std::initializer_list<value_type> v )
  {
    this->separate(ts_now, false);
    if ( ts_now < _next_time - _step_ts )
      return false;
    _reducer.add( std::move(v) );
    return true;
  }
  
  bool add( const reduced_data& v )
  {
    this->separate( v.ts, false);
    if ( v.ts < _next_time - _step_ts )
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
  std::list<reduced_ptr> _sep_list;
};


}

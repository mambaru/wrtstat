#pragma once
#include <wrtstat/reducer.hpp>
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
  
  separator(time_type now, time_type step, size_t limit, size_t levels, pool::allocator allocator = pool::allocator())
    : _reducer(limit, levels, allocator)
    , _step_ts(step)
    , _next_time(now + step)
  {
  }

  void set_time(time_type now) 
  {
    _next_time = now + _step_ts;
  }
  
  const reducer_type& get_reducer() const { return this->_reducer;}

  bool add(time_type now, value_type v)
  {
    auto ready = this->separate(now, false);
    _reducer.add(v);
    return ready;
  }

  reduced_ptr add_and_pop(time_type now, value_type v)
  {
    auto res = this->separate_and_pop(now, v);
    _reducer.add(v);
    return res;
  }

  bool separate(time_type now, bool force)
  {
    if ( !force && now < _next_time )
      return false;
    _next_time += _step_ts;
    if ( auto res = _reducer.detach() )
    {
      _sep_list.push_back( std::move(res) );
    }
    return true;
  }
  
  reduced_ptr separate_and_pop(time_type now, bool force)
  {
    if ( !force && now < _next_time )
      return nullptr;
    _next_time += _step_ts;
    return _reducer.detach();
  }

  bool ready() { return !_sep_list.empty();}
  bool empty() { return _sep_list.empty();}
  reduced_ptr pop()
  {
    if ( _sep_list.empty() )
      return nullptr;
    auto res = std::move(_sep_list.front());
    _sep_list.pop_front();
    return res;
  }
  
private:
  reducer_type _reducer;
  time_type _step_ts;
  time_type _next_time;
  std::list<reduced_ptr> _sep_list;
};


}

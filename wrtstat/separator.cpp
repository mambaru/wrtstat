#include <wrtstat/separator.hpp>

namespace wrtstat {

separator::separator( time_type ts_now, const separator_options& opt, const allocator& a )
  : _reducer(opt, a)
  , _step_ts(opt.aggregation_step_ts)
  , _next_time(0)
  , _resolution(opt.resolution)
{
  ts_now = this->get_ts(ts_now);
  if ( opt.soiled_start_ts!=0 )
  {
    size_t rnd = static_cast<size_t>(std::rand());
    ts_now -= time_type(rnd*rnd) % opt.soiled_start_ts;
  }
  _next_time = ts_now + opt.aggregation_step_ts;
}

const separator::reducer_type& separator::get_reducer() const 
{
  return this->_reducer;
}

bool separator::add( time_type ts_now, value_type v, size_type count )
{
  time_type ts = this->get_ts(ts_now);
  this->separate(ts, false);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add(v, count);
  return true;
}
  
bool separator::add( time_type ts_now, const data_type& v, size_type count )
{
  time_type ts = this->get_ts(ts_now);
  this->separate(ts, false);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add(v, count);
  return true;
}
  
bool separator::add( time_type ts_now, std::initializer_list<value_type> v )
{
  time_type ts = this->get_ts(ts_now);
  this->separate(ts, false);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add( std::move(v) );
  return true;
}
  
bool separator::add( const reduced_data& v )
{
  time_type ts = this->get_ts(v.ts);
  this->separate( ts, false);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add( v );
  return true;
}

bool separator::add( const reduced_data& v, aggregated_handler handler )
{
  time_type ts = this->get_ts(v.ts);
  this->separate( ts, handler);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add( v );
  return true;
}

separator::reduced_ptr separator::pop()
{
  if ( _sep_list.empty() )
  return nullptr;
  auto res = std::move(_sep_list.front());
  _sep_list.pop_front();
  return res;
}

separator::reduced_ptr separator::force_pop()
{
  if (auto r = this->pop() )
    return r; 
  auto r = _reducer.detach();
  r->ts = _next_time;
  return r;
}

separator::time_type separator::next_time() const 
{
  return _next_time;
}

separator::time_type separator::current_time() const 
{
  return _next_time - _step_ts;
}

separator::time_type separator::get_ts(time_type ts)
{
  return ts!=0 ? ts : separator::now(_resolution);
}
  
bool separator::separate(time_type ts_now, aggregated_handler handler)
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
  
bool separator::separate(time_type ts_now, bool force)
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
  
bool separator::ready() const
{
  return !_sep_list.empty();
}

bool separator::empty() const
{
  return _sep_list.empty();
}
  
separator::time_type separator::now(time_type resolution)
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

size_t separator::size() const 
{
  return _sep_list.size();
}

void separator::clear( time_type ts_now )
{
  _next_time = ts_now + _step_ts;
  _reducer.clear();
  _sep_list.clear();
}

}

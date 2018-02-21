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

bool separator::add( time_type ts, value_type v, size_type count )
{
  ts = this->get_ts(ts);
  this->separate(ts, false);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add(v, count);
  return true;
}
  
bool separator::add( time_type ts, const data_type& v, size_type count )
{
  ts = this->get_ts(ts);
  this->separate(ts, false);
  if ( ts < _next_time - _step_ts )
    return false;
  _reducer.add(v, count);
  return true;
}
  
bool separator::add( time_type ts, std::initializer_list<value_type> v )
{
  ts = this->get_ts(ts);
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
    return std::move(r); 
  auto r = _reducer.detach();
  r->ts = _next_time  - _step_ts;
  return std::move(r);
}

separator::reduced_ptr separator::get_current()
{
  if ( auto r = _reducer.get_current() )
  {
    r->ts = _next_time  - _step_ts;
    return std::move(r);
  }
  return nullptr;
}

time_type separator::next_time() const 
{
  return _next_time;
}

time_type separator::current_time() const 
{
  return _next_time - _step_ts;
}

time_type separator::get_ts(time_type ts) const
{
  time_type now_ts = ( ts!=0 || _resolution==0 ) ? ts : separator::now(_resolution);
  now_ts /= _step_ts;
  now_ts *= _step_ts;
  return now_ts;
}
  
bool separator::separate(time_type ts_now, aggregated_handler handler)
{
  ts_now = this->get_ts(ts_now);
  
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
  ts_now = this->get_ts(ts_now);
  
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
  
time_type separator::now(time_type resolution)
{
  switch (resolution)
  {
    case 1 : return separator::now_t<std::chrono::seconds>();
    case 1000 : return separator::now_t<std::chrono::milliseconds>();
    case 1000000 : return separator::now_t<std::chrono::microseconds>();
    case 1000000000 : return separator::now_t<std::chrono::nanoseconds>();
    default:
      return 0;
  };
  return 0;
}

time_type separator::now()
{
  return this->now(_resolution);
}

size_t separator::size() const 
{
  return _sep_list.size();
}

void separator::clear( time_type ts_now )
{
  ts_now = this->get_ts(ts_now);
  _next_time = ts_now + _step_ts;
  _reducer.clear();
  _sep_list.clear();
}

}

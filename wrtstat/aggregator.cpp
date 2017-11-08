#include <wrtstat/aggregator.hpp>
#include <wrtstat/separator.hpp>
#include <iostream>
#include <algorithm>

namespace wrtstat {

aggregator_base::aggregator_base(time_type ts_now, const aggregator_options& opt, const allocator& a )
  : _sep(ts_now, opt, a)
  , _reduced_size(opt.outgoing_reduced_size)
  , _id( std::make_shared<int>(1) )
{
}

const separator& aggregator_base::get_separator() const 
{
  return this->_sep;
}


bool aggregator_base::add(time_type ts_now, value_type v, size_type count)
{
  if ( !_enabled )
    return true;

  if ( !_sep.add(ts_now, v, count) )
    return false; // ts_now устарел

  this->aggregate0_();
  return true;
}

bool aggregator_base::add(time_type ts_now, const data_type& v, size_type count)
{
  if ( !_enabled )
    return true;

  if ( !_sep.add(ts_now, v, count) )
    return false; // ts_now устарел

  this->aggregate0_();
  return true;
}

bool aggregator_base::add( const reduced_data& v)
{
  if ( !_enabled )
    return true;
  if ( !_sep.add(v) )
    return false; // v.ts устарел
  this->aggregate0_();
  return true;
}

bool aggregator_base::add( const reduced_data& v, aggregated_handler handler)
{
  if ( !_enabled )
    return true;

  bool result = _sep.add(v, [this, handler](aggregated_ptr ag)
  {
    if ( handler!=nullptr )
    {
      ag = this->aggregate2_( std::move(ag) );
      this->reduce_(ag->data);
      handler(std::move(ag));
    }
  });
    
  return result;
}

bool aggregator_base::separate(time_type ts_now, bool force)
{
  bool res = _sep.separate(ts_now, force);
  this->aggregate0_();
  return res;
}
  
aggregator_base::aggregated_ptr aggregator_base::pop()
{
  if ( _ag_list.empty() ) 
    return nullptr;
  auto res = std::move(_ag_list.front() );
  _ag_list.pop_front();
  return res;
}

aggregator_base::aggregated_ptr aggregator_base::force_pop()
{
  auto ag = this->aggregate2_(_sep.force_pop());
  this->reduce_(ag->data);
  return std::move(ag);
}

void aggregator_base::enable(bool value)
{
  _enabled = value;
}

template<typename T>
//aggregator_base::meter_fun_t aggregator_base::create_meter( std::weak_ptr<T> wthis )
aggregator_base::value_adder_t aggregator_base::create_value_adder( std::weak_ptr<T> wthis )
{
  std::weak_ptr<int> wid = this->_id;
  return [wid, wthis](time_type ts_now, time_type v, size_type count)
  {
    if ( auto pthis = wthis.lock() )
    {
      if (auto id = wid.lock() )
      {
        pthis->add(ts_now, v, count);
      }
    }
  };
}

template<typename T>
//aggregator_base::handler_fun_t aggregator_base::create_handler( std::weak_ptr<T> wthis )
aggregator_base::data_adder_t aggregator_base::create_data_adder( std::weak_ptr<T> wthis )
{
  std::weak_ptr<int> wid = this->_id;
  return [wid, wthis](time_type ts_now, data_type&& v, size_type count)
  {
    if ( auto pthis = wthis.lock() )
    {
      if (auto id = wid.lock() )
      {
        pthis->add(ts_now, std::move(v), count);
      }
    }
  };
}
  
template<typename T>
//aggregator_base::aggregator_fun_t aggregator_base::create_aggregator( std::weak_ptr<T> wthis )
aggregator_base::reduced_adder_t aggregator_base::create_reduced_adder( std::weak_ptr<T> wthis )
{
  std::weak_ptr<int> wid = this->_id;
  return [wid, wthis]( const reduced_data& reduced)
  {
    if ( auto pthis = wthis.lock() )
    {
      if (auto id = wid.lock() )
      {
        pthis->add(reduced);
      }
    }
  };
}

size_t aggregator_base::size() const
{
  return _ag_list.size();
}

void aggregator_base::clear( time_type ts_now )
{
  _sep.clear(ts_now);
  _ag_list.clear();
  _id.reset();
}

value_type aggregator_base::nth_(size_type perc, size_type& off, data_type& d)
{
  data_type::iterator beg = d.begin() + static_cast<std::ptrdiff_t>(off);
  data_type::iterator nth = d.begin() + static_cast<std::ptrdiff_t>( d.size()*perc/100 );
  auto dist = std::distance(beg, nth );
  if ( dist < 0 )
  {
    return *beg; // abort;
  }
  off = static_cast<size_type>(dist);
  std::nth_element(beg, nth, d.end());
  return *nth;
}

void aggregator_base::reduce_(data_type& d) const
{
  if ( _reduced_size==0 || d.size() <= _reduced_size)
    return;

  size_type i = 0;
  auto beg = d.begin();
  auto end = d.end();
  if ( _reduced_size*2 > d.size() )
  {
    // 300 ~ 400
    // Удаляем каждый N элемент
    size_type step = d.size() / ( d.size() - _reduced_size );
    end = std::remove_if( beg, end, [&i, step](const value_type&) {
      return (i++)%step==0;
    });
  }
  else
  {
    // 50 - 400
    // Оставляем каждый N элемент
    size_type step = d.size() / _reduced_size;
    end = std::remove_if( beg, end, [&i, step](const value_type&) {
      return (i++)%step!=0;
    });
  }
  auto dist = std::distance( beg, end);
  if ( dist > 0 )
    d.resize( static_cast<size_t>(dist) );
  else
    abort();
}

void aggregator_base::aggregate0_()
{
  while (auto sep = _sep.pop() )
  {
    aggregated_ptr ag = this->aggregate2_( std::move(sep) );
    this->reduce_(ag->data);
    _ag_list.push_back( std::move(ag) );
  }
}

void aggregator_base::aggregate1_(aggregated_handler handler)
{
  while (auto sep = _sep.pop() )
  {
    aggregated_ptr ag = this->aggregate2_( std::move(sep) );
    this->reduce_(ag->data);
    handler( std::move(ag) );
  }
}

aggregator_base::aggregated_ptr aggregator_base::aggregate2_(reduced_ptr d) const
{
  if ( d==nullptr )
    return nullptr;

  aggregated_ptr& res = d;
  //aggregated_ptr res = aggregated_ptr(new aggregated_type() );
  // static_cast<reduced_data&>(*res) = std::move(*d);
  size_t s = res->data.size();
  if ( s!=0 )
  {
    size_type off = 0;
    res->perc50 = this->nth_(50, off, res->data);
    res->perc80 = this->nth_(80, off, res->data);
    res->perc95 = this->nth_(95, off, res->data);
    res->perc99 = this->nth_(99, off, res->data);
    res->perc100 = res->data[ s - 1 ];
  }
  return std::move(res);
}

  
aggregator::aggregator(time_type ts_now, const options_type& opt, const allocator& a )
  : aggregator_base(ts_now, opt, a)
{
}
  
aggregator::value_adder_t aggregator::create_value_adder( )
{
  return aggregator_base::create_value_adder<aggregator>( this->shared_from_this() );
}

aggregator::data_adder_t aggregator::create_data_adder( )
{
  return aggregator_base::create_data_adder<aggregator>( this->shared_from_this() );
}

aggregator::reduced_adder_t aggregator::create_reduced_adder( )
{
  return aggregator_base::create_reduced_adder<aggregator>( this->shared_from_this() );
}

aggregator_mt::aggregator_mt(time_type ts_now, const options_type& opt, const allocator& a)
  : aggregator_base(ts_now, opt, a)
{
}

bool aggregator_mt::add(time_type ts_now, value_type v, size_type count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::add(ts_now, v, count);
}

bool aggregator_mt::add(time_type ts_now, const data_type& v, size_type count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::add(ts_now, v, count);
}

bool aggregator_mt::add( const reduced_data& v)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::add(v);
}

bool aggregator_mt::add( const reduced_data& v, aggregated_handler handler)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::add(v, handler);
}

aggregator_mt::aggregated_ptr aggregator_mt::pop()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::pop();
}

aggregator_mt::aggregated_ptr aggregator_mt::force_pop()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::force_pop();
}

bool aggregator_mt::separate(time_type ts_now, bool force)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::separate(ts_now, force);
}

void aggregator_mt::enable(bool value)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::enable(value);
}

aggregator_mt::value_adder_t aggregator_mt::create_value_adder( )
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::create_value_adder<aggregator_mt>( this->shared_from_this() );
}

aggregator_mt::data_adder_t aggregator_mt::create_data_adder( )
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::create_data_adder<aggregator_mt>( this->shared_from_this() );
}
  
aggregator_mt::reduced_adder_t aggregator_mt::create_reduced_adder( )
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::create_reduced_adder<aggregator_mt>( this->shared_from_this() );
}

size_t aggregator_mt::size() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::size();
}

void aggregator_mt::clear(time_type ts_now)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return aggregator_base::clear( ts_now );
}

}

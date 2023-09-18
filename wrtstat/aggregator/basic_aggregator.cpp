#include "basic_aggregator.hpp"
#include <algorithm>

namespace wrtstat {

basic_aggregator::basic_aggregator(time_type ts_now, const aggregator_options& opt, const allocator& a )
  : _sep(ts_now, opt, a)
  , _outgoing_reduced_size(opt.outgoing_reduced_size)
  , _id( std::make_shared<int>(1) )
{
}

const separator& basic_aggregator::get_separator() const
{
  return this->_sep;
}

time_type basic_aggregator::now(time_type resolution)
{
  return separator::now(resolution);
}

time_type basic_aggregator::now(resolutions resolution)
{
  return separator::now(resolution);
}

time_type basic_aggregator::now()
{
  return _sep.now();
}

bool basic_aggregator::add(time_type ts_now, value_type v, size_type count)
{
  if ( !_enabled )
    return true;

  if ( !_sep.add(ts_now, v, count) )
    return false; // ts_now устарел

  this->aggregate0_();
  return true;
}

bool basic_aggregator::push( time_type ts_now, value_type v, size_type count, aggregated_handler handler)
{
  if ( !_enabled )
    return true;
  if ( handler == nullptr )
    return this->add(ts_now, v, count);
  return _sep.push(ts_now, v, count, std::bind(&basic_aggregator::push_handler_,  this, std::placeholders::_1, handler));
}


bool basic_aggregator::add(time_type ts_now, const data_type& v, size_type count)
{
  if ( !_enabled )
    return true;

  if ( !_sep.add(ts_now, v, count) )
    return false; // ts_now устарел

  this->aggregate0_();
  return true;
}

bool basic_aggregator::push( time_type ts_now, const data_type& v, size_type count, aggregated_handler handler)
{
  if ( !_enabled )
    return true;

  if ( handler == nullptr )
    return this->add(ts_now, v, count);


  return _sep.push(ts_now, v, count, std::bind(&basic_aggregator::push_handler_,  this, std::placeholders::_1, handler));
}

bool basic_aggregator::add( const reduced_data& v)
{
  if ( !_enabled )
    return true;
  if ( !_sep.add(v) )
    return false; // v.ts устарел
  this->aggregate0_();
  return true;
}

bool basic_aggregator::push( const reduced_data& v, aggregated_handler handler)
{
  if ( !_enabled )
    return true;

  if ( handler == nullptr )
    return this->add(v);

  return _sep.push(v, std::bind(&basic_aggregator::push_handler_,  this, std::placeholders::_1, handler));
}


bool basic_aggregator::separate(bool force)
{
  return this->separate(0 /*now*/, nullptr, force);
}

bool basic_aggregator::separate(time_type ts_now, aggregated_handler handler, bool force)
{
  bool res = _sep.separate(ts_now, handler, force);
  this->aggregate0_();
  return res;
}

basic_aggregator::aggregated_ptr basic_aggregator::pop()
{
  if ( _ag_list.empty() )
    return nullptr;
  auto res = std::move(_ag_list.front() );
  _ag_list.pop_front();
  return res;
}

basic_aggregator::aggregated_ptr basic_aggregator::force_pop()
{
  if ( auto ag = this->aggregate2_(_sep.force_pop()) )
  {
    this->reduce_(ag->data);
    return ag;
  }
  return nullptr;
}

basic_aggregator::aggregated_ptr basic_aggregator::aggregate_current()
{
  if ( auto ag = this->aggregate2_(_sep.get_current()) )
  {
    this->reduce_(ag->data);
    return ag;
  }
  return nullptr;
}

void basic_aggregator::calc_perc(aggregated_data& d)
{
  size_t s = d.data.size();
  if ( s!=0 )
  {
    size_type off = 0;
    d.perc50 = self::nth_(50, off, d.data);
    d.perc80 = self::nth_(80, off, d.data);
    d.perc95 = self::nth_(95, off, d.data);
    d.perc99 = self::nth_(99, off, d.data);
    d.perc100 = d.data[ s - 1 ];
  }
}


void basic_aggregator::enable(bool value)
{
  _enabled = value;
}

size_t basic_aggregator::size() const
{
  return _ag_list.size();
}

void basic_aggregator::clear( time_type ts_now )
{
  _sep.clear(ts_now);
  _ag_list.clear();
  _id.reset();
}

value_type basic_aggregator::nth_(size_type perc, size_type& off, data_type& d)
{
  data_type::iterator beg = d.begin() + static_cast<std::ptrdiff_t>(off);
  data_type::iterator nth = d.begin() + static_cast<std::ptrdiff_t>( d.size()*perc/100 );
  data_type::difference_type dist = std::distance(beg, nth );
  if ( dist < 0 )
    return *beg;

  off = static_cast<size_type>(dist);
  std::nth_element(beg, nth, d.end());
  return *nth;
}

size_t basic_aggregator::reduce_(data_type& d) const
{
  if ( _outgoing_reduced_size==0 || d.size() <= _outgoing_reduced_size)
    return 0;

  size_type i = 0;
  auto beg = d.begin();
  auto end = d.end();
  if ( _outgoing_reduced_size*2 > d.size() )
  {
    // 300 ~ 400
    // Удаляем каждый N элемент
    size_type step = d.size() / ( d.size() - _outgoing_reduced_size );
    end = std::remove_if( beg, end, [&i, step](const value_type&) {
      return (i++)%step==0;
    });
  }
  else
  {
    // 50 - 400
    // Оставляем каждый N элемент
    size_type step = d.size() / _outgoing_reduced_size;
    end = std::remove_if( beg, end, [&i, step](const value_type&) {
      return (i++)%step!=0;
    });
  }
  auto dist = std::distance( beg, end);
  auto deleted = std::distance( end, d.end() );
  if ( dist > 0 && static_cast<size_t>(dist) < d.size()  )
    d.resize( static_cast<size_t>(dist), 0 );

  return static_cast<size_t>(deleted);
}

void basic_aggregator::aggregate0_()
{
  while (auto sep = _sep.pop() )
  {
    aggregated_ptr ag = this->aggregate2_( std::move(sep) );
    this->reduce_(ag->data);
    _ag_list.push_back( std::move(ag) );
  }
}

void basic_aggregator::aggregate1_(aggregated_handler handler)
{
  while (auto sep = _sep.pop() )
  {
    aggregated_ptr ag = this->aggregate2_( std::move(sep) );
    this->reduce_(ag->data);
    handler( std::move(ag) );
  }
}



basic_aggregator::aggregated_ptr basic_aggregator::aggregate2_(reduced_ptr d)
{
  if ( d==nullptr )
    return nullptr;

  aggregated_ptr& res = d;
  size_t s = res->data.size();
  if ( s!=0 )
  {
    size_type off = 0;
    res->perc50 = self::nth_(50, off, res->data);
    res->perc80 = self::nth_(80, off, res->data);
    res->perc95 = self::nth_(95, off, res->data);
    res->perc99 = self::nth_(99, off, res->data);
    res->perc100 = res->data[ s - 1 ];
  }
  return std::move(res);
}

void basic_aggregator::push_handler_( aggregated_ptr ag, aggregated_handler handler)
{
  if ( handler!=nullptr )
  {
    ag = this->aggregate2_( std::move(ag) );
    this->reduce_(ag->data);
    handler(std::move(ag));
  }
}


}

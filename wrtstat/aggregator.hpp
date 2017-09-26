#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/aggregator_options.hpp>
#include <wrtstat/separator.hpp>
#include <iostream>
#include <algorithm>

namespace wrtstat {

class aggregator_base
{
public:
  typedef separator::time_type time_type;
  typedef separator::value_type value_type;
  typedef separator::data_type data_type;
  typedef separator::size_type size_type;
  typedef separator::reduced_type reduced_type;
  typedef separator::reduced_ptr reduced_ptr;
  typedef types::meter_fun_t meter_fun_t;
  typedef types::handler_fun_t handler_fun_t;

  typedef aggregator_options options_type;
  typedef aggregated_data aggregated_type;
  typedef std::unique_ptr<aggregated_type> aggregated_ptr;
  typedef std::list< aggregated_ptr > aggregated_list;
  
  aggregator_base(time_type ts_now, const aggregator_options& opt, const allocator& a = allocator() )
    : _sep(ts_now, opt, a)
    , _reduced_size(opt.reduced_size)
    , _id( std::make_shared<int>(1) )
  {
    
  }

  const separator& get_separator() const 
  {
    return this->_sep;
  }

  bool add(time_type ts_now, value_type v, size_type count)
  {
    if ( !_enabled )
      return true;

    if ( !_sep.add(ts_now, v, count) )
      return false; // ts_now устарел

    this->aggregate_();
    return true;
  }

  bool add(time_type ts_now, data_type&& v, size_type count)
  {
    if ( !_enabled )
      return true;

    if ( !_sep.add(ts_now, std::move(v), count) )
      return false; // ts_now устарел

    this->aggregate_();
    return true;
  }

  bool separate(time_type ts_now, bool force)
  {
    bool res = _sep.separate(ts_now, force);
    this->aggregate_();
    return res;
  }
  
  aggregated_ptr pop()
  {
    if ( _ag_list.empty() ) 
      return nullptr;
    auto res = std::move(_ag_list.front() );
    _ag_list.pop_front();
    return res;
  }

  aggregated_ptr force_pop()
  {
    return this->aggregate_(_sep.force_pop());
  }

  void enable(bool value)
  {
    _enabled = value;
  }

  template<typename T>
  meter_fun_t create_meter( std::weak_ptr<T> wthis )
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
  handler_fun_t create_handler( std::weak_ptr<T> wthis )
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

  size_t size() const
  {
    return _ag_list.size();
  }

  void clear( time_type ts_now )
  {
    _sep.clear(ts_now);
    _ag_list.clear();
    _id.reset();
  }


  template<typename D>
  static time_type now() 
  {
    return separator::now<D>();
  }


private:

  static value_type nth_(size_type perc, size_type& off, data_type& d)
  {
    data_type::iterator beg = d.begin() + static_cast<std::ptrdiff_t>(off);
    data_type::iterator nth = d.begin() + static_cast<std::ptrdiff_t>( d.size()*perc/100 );
    auto dist = std::distance(beg, nth );
    if ( dist < 0 )
    {
      abort();
      return *beg; // abort;
    }
    off = static_cast<size_type>(dist);
    std::nth_element(beg, nth, d.end());
    return *nth;
  }

  void reduce_(data_type& d) const
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
      end = std::remove_if( beg, d.end(), [&i, step](const value_type&) {
        return (i++)%step==0;
      });
    }
    else
    {
      // 50 - 400
      // Оставляем каждый N элемент
      size_type step = d.size() / _reduced_size;
      end = std::remove_if( beg, d.end(), [&i, step](const value_type&) {
        return (i++)%step!=0;
      });
    }
    auto dist = std::distance( beg, end);
    if ( dist > 0 )
      d.resize( static_cast<size_t>(dist) );
    else
      abort();
  }

  void aggregate_()
  {
    while (auto sep = _sep.pop() )
    {
      aggregated_ptr ag = this->aggregate_( std::move(sep) );
      this->reduce_(ag->data);
      _ag_list.push_back( std::move(ag) );
    }
  }

  aggregated_ptr aggregate_(reduced_ptr d) const
  {
    if ( d==nullptr )
      return nullptr;

    aggregated_ptr& res = d;
    //aggregated_ptr res = aggregated_ptr(new aggregated_type() );
    // static_cast<reduced_data&>(*res) = std::move(*d);
    size_t size = res->data.size();
    if ( size!=0 )
    {
      size_type off = 0;
      res->perc50 = this->nth_(50, off, res->data);
      res->perc80 = this->nth_(80, off, res->data);
      res->perc95 = this->nth_(95, off, res->data);
      res->perc99 = this->nth_(99, off, res->data);
      res->perc100 = res->data[ size - 1 ];
    }
    return std::move(res);
  }

public:
  separator _sep;
  size_t _reduced_size = 0;
  bool _enabled = true;
  aggregated_list _ag_list;
  std::shared_ptr<int> _id;
};

class aggregator
  : public aggregator_base
  , public std::enable_shared_from_this<aggregator>
{
public:
  typedef aggregator_base::options_type options_type;
  
  aggregator(time_type ts_now, const options_type& opt, const allocator& a = allocator())
    : aggregator_base(ts_now, opt, a)
  {
  }
  
  meter_fun_t create_meter( )
  {
    return aggregator_base::create_meter<aggregator>( this->shared_from_this() );
  }

  handler_fun_t create_handler( )
  {
    return aggregator_base::create_handler<aggregator>( this->shared_from_this() );
  }
  
};

class aggregator_mt
  : private aggregator_base
  , public std::enable_shared_from_this<aggregator_mt>
{
public:
  typedef std::mutex mutex_type;
  
  typedef aggregator_base::time_type time_type;
  typedef aggregator_base::value_type value_type;
  typedef aggregator_base::data_type data_type;
  typedef aggregator_base::size_type size_type;
  typedef aggregator_base::reduced_type reduced_type;
  typedef aggregator_base::reduced_ptr reduced_ptr;
  typedef aggregator_base::meter_fun_t meter_fun_t;
  typedef aggregator_base::handler_fun_t handler_fun_t;
  

  typedef aggregator_base::options_type options_type;
  typedef aggregator_base::aggregated_type aggregated_type;
  typedef aggregator_base::aggregated_ptr aggregated_ptr;

  aggregator_mt(time_type ts_now, const options_type& opt, const allocator& a = allocator() )
    : aggregator_base(ts_now, opt, a)
  {
  }

  bool add(time_type ts_now, value_type v, size_type count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::add(ts_now, v, count);
  }

  bool add(time_type ts_now, data_type&& v, size_type count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::add(ts_now, std::move(v), count);
  }

  
  aggregated_ptr pop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::pop();
  }

  aggregated_ptr force_pop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::force_pop();
  }

  bool separate(time_type ts_now, bool force)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::separate(ts_now, force);
  }

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::enable(value);
  }

  meter_fun_t create_meter( )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::create_meter<aggregator_mt>( this->shared_from_this() );
  }

  handler_fun_t create_handler( )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::create_handler<aggregator_mt>( this->shared_from_this() );
  }

  template<typename D>
  static time_type now() 
  {
    return aggregator_base::now<D>();
  }

  size_t size() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::size();
  }

  void clear(time_type ts_now)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return aggregator_base::clear( ts_now );
  }

private:
  mutable mutex_type _mutex;
};

}

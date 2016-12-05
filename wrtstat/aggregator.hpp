#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/aggregator_options.hpp>
#include <wrtstat/separator.hpp>
#include <iostream>
namespace wrtstat {

class aggregator
  : public std::enable_shared_from_this<aggregator>
{
public:
  typedef separator::time_type time_type;
  typedef separator::value_type value_type;
  typedef separator::data_type data_type;
  typedef separator::size_type size_type;
  typedef separator::reduced_type reduced_type;
  typedef separator::reduced_ptr reduced_ptr;
  typedef types::set_span_fun_t set_span_fun_t;

  typedef aggregator_options options_type;
  typedef aggregated_data aggregated_type;
  typedef std::unique_ptr<aggregated_type> aggregated_ptr;
  typedef std::list< aggregated_ptr > aggregated_list;
  
  aggregator(time_type now, aggregator_options opt, pool::allocator allocator = pool::allocator())
    : _sep(now, opt, allocator)
    , _reduced_size(opt.reduced_size)
  {
    _id = std::make_shared<int>(1);
  }

  bool add(time_type now, value_type v, size_type count)
  {
    if ( !_enabled )
      return false;

    if ( !_sep.add(now, v, count) )
      return false;
    while (auto sep = _sep.pop() )
    {
      aggregated_ptr ag = this->aggregate_(std::move(sep) );
      this->reduce_(ag->data);
      _ag_list.push_back( std::move(ag) );
    }
    return true;
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
  
  set_span_fun_t create_meter( )
  {
    std::weak_ptr<aggregator> wthis = this->shared_from_this();
    std::weak_ptr<int> wid = this->_id;
    return [wid, wthis](time_type now, time_type v, size_type count)
    {
      if ( auto pthis = wthis.lock() )
      {
        if (auto id = wid.lock() )
        {
          pthis->add(now, v, count);
        }
      }
    };
  }

  void enable(bool value)
  {
    _enabled = value;
  }

private:

  value_type nth_(size_type perc, size_type& off, data_type& d) const
  {
    auto beg = d.begin() + off;
    auto nth = d.begin() + d.size()*perc/100;
    off = std::distance(beg, nth);
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
    d.resize(std::distance( beg, end));
  }
  
  aggregated_ptr aggregate_(reduced_ptr d) const
  {
    if ( d==nullptr )
      return nullptr;

    aggregated_ptr res = aggregated_ptr(new aggregated_type() );
    static_cast<reduced_data&>(*res) = std::move(*d);
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

    return res;
  }

public:
  separator _sep;
  size_t _reduced_size = 0;
  bool _enabled = true;
  aggregated_list _ag_list;
  std::shared_ptr<int> _id;
};

}

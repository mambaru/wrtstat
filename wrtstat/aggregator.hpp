#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/separator.hpp>

namespace wrtstat {

class aggregator
{
public:
  typedef separator::time_type time_type;
  typedef separator::value_type value_type;
  typedef aggregated_data aggregated_type;
  typedef std::unique_ptr<aggregated_type> aggregated_ptr;
  typedef std::list< aggregated_ptr > aggregated_list;
  
  aggregator(time_type now, time_type step, size_t limit, size_t levels, size_t reserve=0)
    : _sep(now, step, limit, levels, reserve)
  {
  }

  void set_limit(size_t aggregated_size)
  {
    _aggregated_size = aggregated_size;
  }

  bool add(time_type now, value_type v)
  {
    if ( !_sep.add(now, v) )
      return false;
    while (auto res = _sep.pop() )
    {
      aggregated_ptr d = aggregated_ptr(new aggregated_type);
      static_cast<reduced_data&>(*d) = std::move(*res);
      size_t size = d->data.size();
      if ( size!=0 )
      {
        d->perc50 = d->data[ (size *50)/100 ];
        d->perc80 = d->data[ (size *80)/100 ];
        d->perc95 = d->data[ (size *95)/100 ];
        d->perc99 = d->data[ (size *99)/100 ];
        d->perc100 = d->data[ size - 1 ];
      }
      _ag_list.push_back( std::move(d) );
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

public:
  size_t _aggregated_size = 0;
  separator _sep;
  aggregated_list _ag_list;
};

}

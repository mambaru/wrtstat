#pragma once
#include <wrtstat/reduced_data.hpp>
#include <memory>
#include <functional>
#include <algorithm>

namespace wrtstat {

struct aggregated_perc
{
  value_type perc50  = 0;
  value_type perc80  = 0;
  value_type perc95  = 0;
  value_type perc99  = 0;
  value_type perc100 = 0;
};

struct aggregated_info
  : reduced_info
  , aggregated_perc
{
  aggregated_info() = default;
  aggregated_info& operator+=(const aggregated_info& value);
};

inline aggregated_info& aggregated_info::operator+=(const aggregated_info& v)
{
  reduced_info::operator+=(v);
  typedef std::pair<value_type, value_type> vp_t;
  std::array< vp_t, 10 > arr1 = 
  {
    vp_t{ value_type(perc50*(count/2+1) ),    perc50},
    vp_t{ value_type(perc80*(count/3+1) ),    perc80},
    vp_t{ value_type(perc95*(count/6+1) ),    perc95},
    vp_t{ value_type(perc99*(count/10+1) ),   perc99},
    vp_t{ value_type(perc100*(count/100+1) ), perc100},
    vp_t{ value_type(v.perc50*(v.count/2+1) ),    v.perc50},
    vp_t{ value_type(v.perc80*(v.count/3+1) ),    v.perc80},
    vp_t{ value_type(v.perc95*(v.count/6+1) ),    v.perc95},
    vp_t{ value_type(v.perc99*(v.count/10+1) ),   v.perc99},
    vp_t{ value_type(v.perc100*(v.count/100+1) ), v.perc100}
  };
  
  std::sort(arr1.begin(), arr1.end());
  std::array< value_type, 5 > arr2 = { arr1[3].second, arr1[6].second, arr1[7].second, arr1[8].second, arr1[9].second};
  std::sort(arr2.begin(), arr2.end());
  perc50 = arr2[0];
  perc80 = arr2[1];
  perc95 = arr2[2];
  perc99 = arr2[3];
  perc100 = arr2[4];
  
  return *this;
}
  
struct aggregated_data
  : reduced_data
  , aggregated_perc
{
  typedef std::unique_ptr<aggregated_data> ptr;
  typedef std::function< void(ptr) > handler;
};

}

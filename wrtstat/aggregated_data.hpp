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
  aggregated_info& operator+=(const aggregated_info& v);
};

inline aggregated_info& aggregated_info::operator+=(const aggregated_info& v)
{
  reduced_info::operator+=(v);
  typedef std::pair<value_type, value_type> vp_t;
  value_type count1 = static_cast<value_type>(this->count);
  value_type count2 = static_cast<value_type>(v.count);
  std::array< vp_t, 10 > arr1;
  
  arr1[0].first = perc50*(count1/2+1);     arr1[0].second = perc50;
  arr1[1].first = perc80*(count1/3+1);     arr1[1].second = perc80;
  arr1[2].first = perc95*(count1/6+1);     arr1[2].second = perc95;
  arr1[3].first = perc99*(count1/10+1);     arr1[3].second = perc99;
  arr1[4].first = perc100*(count1/100+1);     arr1[4].second = perc100;
  
  arr1[5].first = v.perc50*(count2/2+1);     arr1[5].second = v.perc50;
  arr1[6].first = v.perc80*(count2/3+1);     arr1[6].second = v.perc80;
  arr1[7].first = v.perc95*(count2/6+1);     arr1[7].second = v.perc95;
  arr1[8].first = v.perc99*(count2/10+1);     arr1[8].second = v.perc99;
  arr1[9].first = v.perc100*(count2/100+1);     arr1[9].second = v.perc100;
  
  std::sort(arr1.begin(), arr1.end());
  std::array< value_type, 5 > arr2;
  arr2[0] = arr1[3].second;
  arr2[1] = arr1[6].second; 
  arr2[2] = arr1[7].second; 
  arr2[3] = arr1[8].second;
  arr2[4] = arr1[9].second;
  
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

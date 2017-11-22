#include <fas/testing.hpp>
#include <wrtstat/wrtstat.hpp>
#include <chrono>

namespace {

UNIT(wrtstat1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;
  wrtstat_mt stat;
  stat.create_multi_meter<std::chrono::microseconds>(
    "a1", "a2", "a3", 
    wrtstat_mt::now_t<std::chrono::microseconds>(), 
    static_cast<size_type >(255)
  );
  
  stat.create_multi_meter< size_meter >( "s1", wrtstat_mt::now_t<std::chrono::microseconds>(), 12);
  stat.create_multi_meter< value_meter >( "v1", wrtstat_mt::now_t<std::chrono::microseconds>(), 12);
  stat.create_multi_meter< value_meter >( "v2", wrtstat_mt::now_t<std::chrono::microseconds>(), 12, 1);
  
  stat.create_multi_meter< time_meter<std::chrono::nanoseconds> >( "t1", wrtstat_mt::now_t<std::chrono::microseconds>());
  stat.create_multi_meter< time_meter<std::chrono::nanoseconds> >( "t2", wrtstat_mt::now_t<std::chrono::microseconds>(), 10 );
  t << nothing;
}

}

BEGIN_SUITE(wrtstat, "")
  ADD_UNIT(wrtstat1)
END_SUITE(wrtstat)


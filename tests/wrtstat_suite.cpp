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

UNIT(wrtstat2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;
  
  wrtstat_mt::options_type opt;
  wrtstat_mt stat(opt);
  wrtstat::id_t id = stat.create_aggregator( "test1", 0 );
  // bool add(id_t id, time_type ts_now, value_type v, size_type cnt)
  stat.add(id, 0, 0, 1);
  stat.add(id, 1, 0, 1);
  auto pag = stat.pop(id);
  t << is_true<assert>(pag!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, value_type>(pag->count, 1) << FAS_FL;
  t << nothing;
}

}

BEGIN_SUITE(wrtstat, "")
  ADD_UNIT(wrtstat1)
  ADD_UNIT(wrtstat2)
END_SUITE(wrtstat)


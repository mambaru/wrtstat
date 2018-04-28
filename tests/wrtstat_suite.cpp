#include <fas/testing.hpp>
#include <wrtstat/meter_manager.hpp>
#include <chrono>

namespace {

UNIT(wrtstat1, "")
{
  using namespace fas::testing;
  wrtstat::meter_manager stat;
  stat.create_multi_meter<std::chrono::microseconds>(
    "a1", "a2", "a3", 
    wrtstat::meter_manager::now_t<std::chrono::microseconds>(), 
    static_cast<wrtstat::size_type >(255)
  );
  
  stat.create_multi_meter< wrtstat::size_meter >( "s1", wrtstat::meter_manager::now_t<std::chrono::microseconds>(), 12);
  stat.create_multi_meter< wrtstat::value_meter >( "v1", wrtstat::meter_manager::now_t<std::chrono::microseconds>(), 12);
  stat.create_multi_meter< wrtstat::value_meter >( "v2", wrtstat::meter_manager::now_t<std::chrono::microseconds>(), 12, 1);
  
  stat.create_multi_meter< wrtstat::time_meter<std::chrono::nanoseconds> >( "t1", wrtstat::meter_manager::now_t<std::chrono::microseconds>());
  stat.create_multi_meter< wrtstat::time_meter<std::chrono::nanoseconds> >( "t2", wrtstat::meter_manager::now_t<std::chrono::microseconds>(), 10 );
  t << nothing;
}

UNIT(wrtstat2, "")
{
  using namespace fas::testing;
  
  wrtstat::meter_manager::options_type opt;
  opt.resolution = 0;
  wrtstat::meter_manager stat(opt);
  wrtstat::id_t id = stat.create_aggregator( "test1", 0 );
  // bool add(id_t id, time_type ts_now, value_type v, size_type cnt)
  stat.add(id, 0, 0, 1);
  stat.add(id, 1, 0, 1);
  auto pag = stat.pop(id);
  t << is_true<assert>(pag!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, wrtstat::value_type>(pag->count, 1) << FAS_FL;
  t << nothing;
}

}

BEGIN_SUITE(wrtstat, "")
  ADD_UNIT(wrtstat1)
  ADD_UNIT(wrtstat2)
END_SUITE(wrtstat)


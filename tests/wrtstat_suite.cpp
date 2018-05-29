#include <fas/testing.hpp>
#include <wrtstat/meter_manager.hpp>
#include <chrono>
#include <map>

#define COUNT 1000000
namespace {

UNIT(wrtstat1, "")
{
  using namespace fas::testing;
  wrtstat::meter_manager stat;
  stat.create_multi_meter<std::chrono::microseconds>(
    "a1", "a2", "a3", 
    wrtstat::meter_manager::now_t<std::chrono::microseconds>(), 
    static_cast<wrtstat::size_type >(255),
    0, 0, true
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
  opt.resolution = wrtstat::resolutions::none;
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

template<typename ... Args>
void unused_params(const Args& ...)
{
  
}

UNIT(wrtstat3, "")
{
  using namespace fas::testing;
  t << flush;
  wrtstat::meter_manager::options_type opt;
  opt.resolution = wrtstat::resolutions::microseconds;
  opt.aggregation_step_ts = 1000000;
  /*opt.reducer_limit = 256;
  opt.reducer_levels = 256;
  opt.pool_size = 256;*/
  opt.prefixes={"a~~"/*, "b~~", "c~~", "d~~", "e~~", "f~~"*/};
  
  int test = 0;
//#error TODO передавать имя!! и убрать удаление счетчиков, иначе ссылка будет не дествительна, а по значению захватывать не эффективно 
  opt.handler=[&test, &t](const std::string& name, wrtstat::aggregated_data::ptr ag){
    ++test ;
    unused_params(t, name, ag);
    t << message(name) << " count=" << ag->count << " 80%=" << ag->perc80 << " 100%=" << ag->perc100 << " max=" << ag->max << " lossy=" << ag->lossy;
  };
  //opt.handler=[](const std::string& , wrtstat::aggregated_data::ptr ) {};
  wrtstat::meter_manager stat(opt);
  
  std::map<size_t, size_t> test_map;
  //auto meter = stat.create_multi_meter_factory<std::chrono::nanoseconds>("time1", "size1", "size2", true);
  auto meter = stat.create_composite_multi_meter_factory<std::chrono::nanoseconds>("", "", "size2", true);
  //auto meter = stat.create_multi_meter_factory<std::chrono::nanoseconds>("time1");
  //auto meter = stat.create_composite_meter_factory<std::chrono::nanoseconds>("time1", "size1", "size2", false);
  //auto meter = stat.create_time_meter_factory<std::chrono::microseconds>("time1");
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0 ; i < COUNT; ++i)
  {
    auto m = meter.create(1UL, size_t(std::rand()%100), 0UL);
    //auto m = meter->create(1UL, 0UL, 0UL);
    for (int j=0;j<10; ++j)
      test_map[i]++;
    m.set_write_size(size_t(std::rand()%1000));
    //m.reset();
  }
  auto finish = std::chrono::steady_clock::now();
  time_t span_mks = std::chrono::duration_cast<std::chrono::microseconds>(finish -start).count();
  t << message("COUNT=") << COUNT << " tests=" << test << " time=" << span_mks << "mks rate=" << (COUNT*1000000L)/span_mks;
  //t << equal<assert>(test, 10) << FAS_FL;
}

}

BEGIN_SUITE(wrtstat, "")
  ADD_UNIT(wrtstat1)
  ADD_UNIT(wrtstat2)
  ADD_UNIT(wrtstat3)
END_SUITE(wrtstat)


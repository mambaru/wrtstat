#include <fas/testing.hpp>
#include <wrtstat/wrtstat.hpp>
#include <chrono>
#include <map>

//#define COUNT 1000000
#define COUNT 1
namespace {

UNIT(wrtstat1, "")
{
  using namespace fas::testing;

  wrtstat::wrtstat::options_type options;
  // Если resolutions now_t не совпадает, то
  options.resolution = wrtstat::resolutions::microseconds;

  wrtstat::wrtstat stat(options);
  stat.create_composite_multi_meter<std::chrono::microseconds>(
    {},
    "a1", "a2", "a3", true).create(
    static_cast<wrtstat::size_type >(255),
    0l, 0l
  );
  t << message("DEBUG");
  t << flush;

  stat.create_size_multi_meter( {}, "s1").create(12);
  stat.create_value_multi_meter( {}, "v1").create(12, 0u);
  stat.create_value_multi_meter( {}, "v2").create(12, 1u);

  stat.create_time_multi_meter< std::chrono::nanoseconds >( {}, "t1")
      .create(1u);
  stat.create_time_multi_meter< std::chrono::nanoseconds >( {}, "t2")
      .create(10u);
  t << nothing;
}

UNIT(wrtstat2, "")
{
  using namespace fas::testing;

  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::none;
  wrtstat::aggregator_registry stat(opt/*, opt.pool_size, opt.id_init, opt.id_step*/);
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
  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::microseconds;
  opt.aggregation_step_ts = 1000000;
  /*opt.reducer_limit = 256;
  opt.reducer_levels = 256;
  opt.pool_size = 256;*/
  wrtstat::wrtstat::prefix_list_t prefixes={"a~~"/*, "b~~", "c~~", "d~~", "e~~", "f~~"*/};

  int test = 0;
//#error TODO передавать имя!! и убрать удаление счетчиков, иначе ссылка будет не дествительна, а по значению захватывать не эффективно
  opt.handler=[&test, &t](const std::string& name, wrtstat::aggregated_data::ptr ag){
    ++test ;
    unused_params(t, name, ag);
    t << message(name) << " count=" << ag->count << " 80%=" << ag->perc80 << " 100%=" << ag->perc100 << " max=" << ag->max << " lossy=" << ag->lossy;
  };
  //opt.handler=[](const std::string& , wrtstat::aggregated_data::ptr ) {};
  wrtstat::wrtstat stat(opt);

  std::map<size_t, size_t> test_map;
  //auto meter = stat.create_multi_meter_factory<std::chrono::nanoseconds>("time1", "size1", "size2", true);
  auto meter = stat.create_composite_multi_meter<std::chrono::nanoseconds>(prefixes, "", "", "size2", true);
  //auto meter = stat.create_multi_meter_factory<std::chrono::nanoseconds>("time1");
  //auto meter = stat.create_composite_meter_factory<std::chrono::nanoseconds>("time1", "size1", "size2", false);
  //auto meter = stat.create_time_meter_factory<std::chrono::microseconds>("time1");
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0 ; i < COUNT; ++i)
  {
    auto m = meter.create(1UL, std::rand()%100, 0);
    //auto m = meter->create(1UL, 0UL, 0UL);
    for (int j=0;j<10; ++j)
      test_map[i]++;
    m.set_write_size(std::rand()%1000);
    //m.reset();
  }
  auto finish = std::chrono::steady_clock::now();
  time_t span_mks = std::chrono::duration_cast<std::chrono::microseconds>(finish -start).count();
  t << message("COUNT=") << COUNT << " tests=" << test << " time=" << span_mks << "mks rate=" << (COUNT*1000000L)/span_mks;
  //t << equal<assert>(test, 10) << FAS_FL;
}


UNIT(wrtstat4, "")
{
  using namespace fas::testing;
  t << flush;
  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::microseconds;
  opt.aggregation_step_ts = 1000000;
  opt.reducer_limit = 256;
  opt.reducer_levels = 256;
  opt.pool_size = 256;
  wrtstat::wrtstat::prefix_list_t prefixes={"a~~", "b~~", "c~~", "d~~", "e~~", "f~~"};

  int test = 0;
//#error TODO передавать имя!! и убрать удаление счетчиков, иначе ссылка будет не дествительна, а по значению захватывать не эффективно
  opt.handler=[&test, &t](const std::string& name, wrtstat::aggregated_data::ptr ag){
    ++test ;
    unused_params(t, name, ag);
    t << message(name) << " count=" << ag->count << " 80%=" << ag->perc80 << " 100%=" << ag->perc100 << " max=" << ag->max << " lossy=" << ag->lossy;
  };
  //opt.handler=[](const std::string& , wrtstat::aggregated_data::ptr ) {};
  wrtstat::wrtstat stat(opt);

  std::map<size_t, size_t> test_map;
  //auto meter = stat.create_multi_meter_factory<std::chrono::nanoseconds>("time1", "size1", "size2", true);
  auto meter = stat.create_composite_multi_meter<std::chrono::nanoseconds>(prefixes, "", "", "size2", true);
  /*auto meter = stat.create_multi_meter_factory<std::chrono::nanoseconds>("time1");
  auto meter = stat.create_composite_meter_factory<std::chrono::nanoseconds>("time1", "size1", "size2", false);
  auto meter = stat.create_time_meter_factory<std::chrono::microseconds>("time1");*/
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0 ; i < COUNT; ++i)
  {
    auto m = meter.create(1UL, std::rand()%100, 0);
    //auto m = meter->create(1UL, 0UL, 0UL);
    for (int j=0;j<10; ++j)
      test_map[i]++;
    m.set_write_size(std::rand()%1000);
    //m.reset();
  }

  auto finish = std::chrono::steady_clock::now();
  time_t span_mks = std::chrono::duration_cast<std::chrono::microseconds>(finish -start).count();
  t << message("COUNT=") << COUNT << " tests=" << test << " time=" << span_mks << "mks rate=" << (COUNT*1000000L)/span_mks;
  t << equal<assert>( stat.aggregators_count(), 7ul) << FAS_FL;

  wrtstat::aggregator_registry::options_type opt2;
  opt2.pool_size = 0;
  opt2.id_init = 0;
  opt2.id_step = 0;
  wrtstat::aggregator_registry ar(opt2/*, 0,0,0*/);
  t << equal<assert>( ar.aggregators_count(), 0ul) << FAS_FL;
}

}

BEGIN_SUITE(wrtstat, "")
  ADD_UNIT(wrtstat1)
  ADD_UNIT(wrtstat2)
  ADD_UNIT(wrtstat3)
  ADD_UNIT(wrtstat4)
END_SUITE(wrtstat)


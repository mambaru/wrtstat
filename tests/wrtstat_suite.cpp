#include <fas/testing.hpp>
#include <wrtstat/wrtstat.hpp>
#include <chrono>
#include <map>
#include <set>
#include <string>

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

struct handler_state
{
  int call_count = 0;
  std::set<std::string> names;
};

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

  handler_state state;
  opt.handler=[&state, &t](const std::string& name, wrtstat::aggregated_data::ptr ag) noexcept {
    ++state.call_count;
    state.names.insert(name);
    t << is_false<assert>( name.empty() ) << FAS_FL;
    t << message(name) << " count=" << ag->count << " 80%=" << ag->perc80 << " 100%=" << ag->perc100 << " max=" << ag->max << " lossy=" << ag->lossy;
  };
  wrtstat::wrtstat stat(opt);

  std::map<size_t, size_t> test_map;
  auto meter = stat.create_composite_multi_meter<std::chrono::nanoseconds>(prefixes, "", "", "size2", true);
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0 ; i < COUNT; ++i)
  {
    auto m = meter.create_shared(1UL, std::rand()%100, 0);
    for (int j=0;j<10; ++j)
      test_map[i]++;
    m->set_write_size(std::rand()%1000);
  }
  stat.force_pushout();
  auto finish = std::chrono::steady_clock::now();
  if ( time_t span_mks = std::chrono::duration_cast<std::chrono::microseconds>(finish -start).count() )
    t << message("COUNT=") << COUNT << " tests=" << state.call_count << " time=" << span_mks << "mks rate=" << (COUNT*1000000L)/span_mks;
  t << is_false<assert>( state.names.empty() ) << FAS_FL;
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

  handler_state state;
  opt.handler=[&state, &t](const std::string& name, wrtstat::aggregated_data::ptr ag) noexcept {
    ++state.call_count;
    state.names.insert(name);
    t << is_false<assert>( name.empty() ) << FAS_FL;
    t << message(name) << " count=" << ag->count << " 80%=" << ag->perc80 << " 100%=" << ag->perc100 << " max=" << ag->max << " lossy=" << ag->lossy;
  };
  wrtstat::wrtstat stat(opt);

  std::map<size_t, size_t> test_map;
  auto meter = stat.create_composite_multi_meter<std::chrono::nanoseconds>(prefixes, "", "", "size2", true);
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0 ; i < COUNT; ++i)
  {
    auto m = meter.create_shared(1UL, std::rand()%100, 0);
    for (int j=0;j<10; ++j)
      test_map[i]++;
    m->set_write_size(std::rand()%1000);
  }
  stat.force_pushout();

  auto finish = std::chrono::steady_clock::now();
  if ( time_t span_mks = std::chrono::duration_cast<std::chrono::microseconds>(finish -start).count() )
    t << message("COUNT=") << COUNT << " tests=" << state.call_count << " time=" << span_mks << "mks rate=" << (COUNT*1000000L)/span_mks;
  t << equal<assert>( stat.aggregators_count(), 7ul) << FAS_FL;
  t << is_false<assert>( state.names.empty() ) << FAS_FL;

  wrtstat::aggregator_registry::options_type opt2;
  opt2.pool_size = 0;
  opt2.id_init = 0;
  opt2.id_step = 0;
  wrtstat::aggregator_registry ar(opt2/*, 0,0,0*/);
  t << equal<assert>( ar.aggregators_count(), 0ul) << FAS_FL;
}

UNIT(wrtstat_del_pop_all, "")
{
  using namespace fas::testing;

  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::none;
  opt.aggregation_step_ts = 10;
  opt.handler = [](const std::string&, wrtstat::aggregated_data::ptr) noexcept {};
  wrtstat::wrtstat stat(opt);

  wrtstat::id_t id1 = stat.create_aggregator("metric_a", 0);
  wrtstat::id_t id2 = stat.create_aggregator("metric_b", 0);
  t << equal<expect>( stat.get_name(id1), "metric_a" ) << FAS_FL;
  t << equal<expect>( stat.get_name(id2), "metric_b" ) << FAS_FL;

  for (int i = 0; i < 10; ++i)
  {
    stat.add(id1, i, static_cast<wrtstat::value_type>(i), 1);
    stat.add(id2, i, static_cast<wrtstat::value_type>(i * 10), 1);
  }

  wrtstat::wrtstat::named_aggregated_list list;
  stat.force_pop_all(&list);
  t << equal<expect, size_t>( list.size(), 2ul ) << FAS_FL;

  stat.del("metric_a");
  t << is_false<assert>( stat.del("metric_a") ) << FAS_FL;
  t << is_false<assert>( stat.add(id1, 0, 1, 1) ) << FAS_FL;

  wrtstat::reduced_data rd;
  rd.ts = 0;
  rd.data = {1, 2, 3};
  rd.count = 3;
  t << is_true<assert>( stat.add("metric_c", rd) ) << FAS_FL;
  t << nothing;
}

UNIT(wrtstat_enable, "")
{
  using namespace fas::testing;

  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::none;
  opt.handler = [](const std::string&, wrtstat::aggregated_data::ptr) noexcept {};
  wrtstat::wrtstat stat(opt);

  wrtstat::id_t id = stat.create_aggregator("disabled_metric", 0);
  t << is_true<assert>( stat.enabled() ) << FAS_FL;
  stat.enable(false);
  t << is_false<assert>( stat.enabled() ) << FAS_FL;
  t << is_true<assert>( stat.add(id, 0, 42, 1) ) << FAS_FL;
  t << is_true<assert>( stat.force_pop(id) == nullptr ) << FAS_FL;

  stat.enable(true);
  t << is_true<assert>( stat.enabled() ) << FAS_FL;
  t << is_true<assert>( stat.add(id, 0, 42, 1) ) << FAS_FL;
  t << is_true<assert>( stat.force_pop(id) != nullptr ) << FAS_FL;
  t << nothing;
}

UNIT(wrtstat_pushout, "")
{
  using namespace fas::testing;

  handler_state state;
  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::none;
  opt.aggregation_step_ts = 10;
  opt.handler = [&state](const std::string& name, wrtstat::aggregated_data::ptr) noexcept
  {
    ++state.call_count;
    state.names.insert(name);
  };
  wrtstat::wrtstat stat(opt);

  wrtstat::id_t id = stat.create_aggregator("handler_metric", 0);
  for (int i = 0; i < 10; ++i)
    stat.add(id, i, static_cast<wrtstat::value_type>(i), 1);

  t << equal<expect, size_t>( stat.pushout(), 0ul ) << FAS_FL;
  t << equal<expect, size_t>( stat.force_pushout(), 1ul ) << FAS_FL;
  t << equal<expect>( state.call_count, 1 ) << FAS_FL;
  t << is_true<assert>( state.names.count("handler_metric") == 1 ) << FAS_FL;
  t << nothing;
}

UNIT(wrtstat_no_handler, "")
{
  using namespace fas::testing;

  {
    wrtstat::wrtstat::options_type opt;
    opt.resolution = wrtstat::resolutions::none;
    opt.aggregation_step_ts = 10;
    wrtstat::wrtstat stat(opt);

    wrtstat::id_t id = stat.create_aggregator("no_handler_metric", 0);
    for (int i = 0; i < 10; ++i)
      stat.add(id, i, static_cast<wrtstat::value_type>(i), 1);

    t << equal<expect, size_t>( stat.force_pushout(), 1ul ) << FAS_FL;
  }
  t << nothing;
}

}

BEGIN_SUITE(wrtstat, "")
  ADD_UNIT(wrtstat1)
  ADD_UNIT(wrtstat2)
  ADD_UNIT(wrtstat3)
  ADD_UNIT(wrtstat4)
  ADD_UNIT(wrtstat_del_pop_all)
  ADD_UNIT(wrtstat_enable)
  ADD_UNIT(wrtstat_pushout)
  ADD_UNIT(wrtstat_no_handler)
END_SUITE(wrtstat)


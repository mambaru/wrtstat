#include <fas/testing.hpp>
#include <wrtstat/aggregator/aggregator.hpp>
#include <wrtstat/aggregator/aggregator_mt.hpp>
#include <wrtstat/aggregator/api/reduced_info.hpp>
#include <wrtstat/aggregator/api/aggregated_info.hpp>
#include <numeric>

namespace {

UNIT(aggregator1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 10;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;
  std::vector<int> values;
  aggregator ag(0, opt);
  for (int i = 0 ; i < 100; i++)
  {
    t << is_true<expect>( ag.add(i, i, 1) ) << FAS_FL;
    values.push_back(i);
  }
  

  ag.separate(100, nullptr, true);
  t << equal<expect, size_t>( ag.size(), 10 ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    int avg = std::accumulate(values.begin() + i*10, values.begin() + (i+1)*10 , 0) / 10;
    auto s = ag.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->lossy,   0  ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->count,   10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->min,     0 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->perc50,  5 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->perc80,  8 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->perc95,  9 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->perc99,  9 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->perc100, 9 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->max,     9 + i*10 ) << "i=" << FAS_FL;
    t << equal<expect, size_t>( s->avg,     avg ) << "i=" << FAS_FL;
  }

  t << nothing;
}

UNIT(aggregator2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;

  auto ag = std::make_shared<aggregator_mt>(0, opt);
  auto meter = ag->create_simple_pusher(nullptr);
  for (int i = 0 ; i < 100; i++)
  {
    meter(i, i, 1);
    t << equal<expect, size_t>( ag->size(), i/10 ) << FAS_FL;
  }

  ag->separate(100, nullptr, true);
  t << equal<expect, size_t>( ag->size(), 10 ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    auto s = ag->pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, i*10 ) << "i=" << FAS_FL;
  }

  t << nothing;
}

UNIT(aggregator_current, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 10;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;

  aggregator ag(0, opt);
  for (int i = 0; i < 5; ++i)
    t << is_true<expect>( ag.add(i, i, 1) ) << FAS_FL;

  auto cur = ag.aggregate_current();
  t << is_true<assert>( cur != nullptr ) << FAS_FL;
  t << stop;
  t << equal<expect>( cur->ts, 0 ) << FAS_FL;
  t << equal<expect, size_t>( cur->count, 5ul ) << FAS_FL;
  t << equal<expect, size_t>( ag.size(), 0ul ) << FAS_FL;

  for (int i = 5; i < 10; ++i)
    t << is_true<expect>( ag.add(i, i, 1) ) << FAS_FL;
  t << is_true<expect>( ag.separate(10, nullptr, true) ) << FAS_FL;
  t << equal<expect, size_t>( ag.size(), 1ul ) << FAS_FL;
  t << nothing;
}

UNIT(aggregator_outgoing_reduced, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 200;
  opt.outgoing_reduced_size = 4;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;

  aggregator ag(0, opt);
  for (int i = 0; i < 100; ++i)
    t << is_true<expect>( ag.add(0, i, 1) ) << FAS_FL;

  t << is_true<expect>( ag.separate(10, nullptr, true) ) << FAS_FL;
  auto s = ag.pop();
  t << is_true<assert>( s != nullptr ) << FAS_FL;
  t << stop;
  t << equal<expect, size_t>( s->count, 100ul ) << FAS_FL;
  t << is_true<expect>( s->data.size() <= 4ul ) << FAS_FL;
  t << nothing;
}

UNIT(reduced_info_merge, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  reduced_info a;
  a.ts = 10;
  a.count = 5;
  a.min = 3;
  a.max = 7;
  a.avg = 4;

  reduced_info b;
  b.ts = 20;
  b.count = 5;
  b.min = 1;
  b.max = 9;
  b.avg = 6;

  a += b;
  t << equal<expect>( a.ts, 10 ) << FAS_FL;
  t << equal<expect>( a.min, 1 ) << FAS_FL;
  t << equal<expect>( a.max, 9 ) << FAS_FL;
  t << equal<expect, size_t>( a.count, 10ul ) << FAS_FL;
  t << equal<expect, size_t>( a.lossy, 0ul ) << FAS_FL;
  t << equal<expect>( a.avg, 5 ) << FAS_FL;

  reduced_info c;
  c += b;
  t << equal<expect>( c.ts, 20 ) << FAS_FL;
  t << equal<expect, size_t>( c.count, 5ul ) << FAS_FL;
  t << nothing;
}

UNIT(aggregated_info_merge, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregated_info a;
  a.ts = 10;
  a.count = 10;
  a.min = 10;
  a.max = 100;
  a.avg = 50;
  a.perc50 = 50;
  a.perc80 = 80;
  a.perc95 = 95;
  a.perc99 = 99;
  a.perc100 = 100;

  aggregated_info b;
  b.ts = 20;
  b.count = 10;
  b.min = 5;
  b.max = 200;
  b.avg = 150;
  b.perc50 = 150;
  b.perc80 = 180;
  b.perc95 = 190;
  b.perc99 = 195;
  b.perc100 = 200;

  a += b;
  t << equal<expect, size_t>( a.count, 20ul ) << FAS_FL;
  t << equal<expect>( a.min, 5 ) << FAS_FL;
  t << equal<expect>( a.max, 200 ) << FAS_FL;
  t << is_true<expect>( a.perc50 <= a.perc80 ) << FAS_FL;
  t << is_true<expect>( a.perc80 <= a.perc95 ) << FAS_FL;
  t << is_true<expect>( a.perc95 <= a.perc99 ) << FAS_FL;
  t << is_true<expect>( a.perc99 <= a.perc100 ) << FAS_FL;
  t << equal<expect>( a.perc50, 50 ) << FAS_FL;
  t << equal<expect>( a.perc100, 180 ) << FAS_FL;
  t << nothing;
}

}

BEGIN_SUITE(aggregator, "")
  ADD_UNIT(aggregator1)
  ADD_UNIT(aggregator2)
  ADD_UNIT(aggregator_current)
  ADD_UNIT(aggregator_outgoing_reduced)
  ADD_UNIT(reduced_info_merge)
  ADD_UNIT(aggregated_info_merge)
END_SUITE(aggregator)

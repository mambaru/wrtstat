#include <fas/testing.hpp>
#include <wrtstat/aggregator.hpp>

namespace {

UNIT(aggregator1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_options opt;
  opt.levels = 1;
  opt.limit  = 10;
  opt.step_ts = 10;

  aggregator ag(0, opt);
  for (int i = 0 ; i < 100; i++)
    t << is_true<expect>( ag.add(i, i, 1) ) << FAS_FL;

  ag.separate(100, true);
  t << equal<expect>( ag.size(), 10 ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
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
    t << equal<expect, size_t>( s->avg,     8 + i*10 - 1*(i!=0) ) << "i=" << FAS_FL;
  }

  t << nothing;
}

UNIT(aggregator2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_options opt;
  opt.levels = 1;
  opt.limit  = 8;
  opt.step_ts = 10;

  auto ag = std::make_shared<aggregator_mt>(0, opt);
  auto meter = ag->create_meter();
  for (int i = 0 ; i < 100; i++)
  {
    meter(i, i, 1);
    t << equal<expect>( ag->size(), i/10 ) << FAS_FL;
  }

  ag->separate(100, true);
  t << equal<expect>( ag->size(), 10 ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    auto s = ag->pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, i*10 ) << "i=" << FAS_FL;
  }

  t << nothing;
}

}

BEGIN_SUITE(aggregator, "")
  ADD_UNIT(aggregator1)
  ADD_UNIT(aggregator2)
END_SUITE(aggregator)

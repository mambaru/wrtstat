#include <fas/testing.hpp>
#include <wrtstat/aggregator/aggregator.hpp>
#include <wrtstat/multi_aggregator/multi_aggregator.hpp>
#include <numeric>

namespace {

UNIT(multi_aggregator1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;
  multi_aggregator_options opt;
  opt.resolution = resolutions::nanoseconds;
  opt.soiled_start_ts = 1000000000;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 10;
  opt.aggregation_step_ts = 500000000;
  opt.outgoing_reduced_size = 13;
  
  multi_aggregator agh(opt);
  
  reduced_data rd;
  bool run = true;
  size_t while_count = 0;
  size_t ag_count = 0;
  while(run)
  {
    rd.ts=aggregator::now_t<std::chrono::nanoseconds>();
    rd.count = 1;
    agh.push("xxx", rd, [&run, &t, &ag_count]( request::push::ptr ag) noexcept
    {
      ag_count = ag->count;
      t << message("aggregator") << " cout=" << ag->count;
      run = false;
    });
    if (run) ++while_count;
  }
  t << equal<expect>( while_count, ag_count) << FAS_FL;
}

UNIT(multi_aggregator_del, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  multi_aggregator_options opt;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;
  multi_aggregator agh(opt);

  reduced_data rd;
  rd.ts = 0;
  rd.count = 1;
  rd.data = {1};

  agh.push("alive", rd, [](request::push::ptr) noexcept {});
  t << is_true<assert>( agh.del("alive") ) << FAS_FL;
  t << is_false<assert>( agh.del("alive") ) << FAS_FL;
  t << is_false<assert>( agh.del("missing") ) << FAS_FL;
  t << nothing;
}

UNIT(multi_aggregator_pushout, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  multi_aggregator_options opt;
  opt.resolution = resolutions::none;
  opt.reducer_levels = 1;
  opt.reducer_limit = 10;
  opt.aggregation_step_ts = 10;
  multi_aggregator agh(opt);

  reduced_data rd;
  rd.ts = 0;
  rd.count = 1;
  rd.data = {42};

  agh.push("m1", rd, [](request::push::ptr) noexcept {});
  agh.push("m2", rd, [](request::push::ptr) noexcept {});

  size_t pushed = 0;
  agh.force_pushout([&pushed](request::push::ptr p) noexcept
  {
    if (p != nullptr)
      ++pushed;
  });
  t << equal<expect, size_t>( pushed, 2ul ) << FAS_FL;
  t << nothing;
}

}

BEGIN_SUITE(multi_aggregator, "")
  ADD_UNIT(multi_aggregator1)
  ADD_UNIT(multi_aggregator_del)
  ADD_UNIT(multi_aggregator_pushout)
END_SUITE(multi_aggregator)

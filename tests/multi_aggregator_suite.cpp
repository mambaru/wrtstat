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
    agh.push("xxx", rd, [&run, &t, &ag_count]( request::push::ptr ag)
    {
      ag_count = ag->count;
      t << message("aggregator") << " cout=" << ag->count;
      run = false;
    });
    if (run) ++while_count;
  }
  t << equal<expect>( while_count, ag_count) << FAS_FL;
}

}

BEGIN_SUITE(multi_aggregator, "")
  ADD_UNIT(multi_aggregator1)
END_SUITE(multi_aggregator)

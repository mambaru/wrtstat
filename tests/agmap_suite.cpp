#include <fas/testing.hpp>
#include <wrtstat/manager/aggregator_map.hpp>
#include <wrtstat/manager/aggregator_hashmap.hpp>
#include <numeric>

namespace {
  

UNIT(agmap1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  agmap_options opt;
  opt.resolution = resolutions::nanoseconds;
  opt.soiled_start_ts = 1000000000;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 10;
  opt.aggregation_step_ts = 500000000;
  opt.outgoing_reduced_size = 13;
  
  aggregator_hashmap agh(opt);
  
  reduced_data rd;
  bool run = true;
  while(run)
  {
    rd.ts=aggregator::now_t<std::chrono::nanoseconds>();
    rd.count = 1;
    agh.push("xxx", rd, [&run, &t]( aggregator_hashmap::aggregated_ptr ag)
    {
      t << message("aggregator") << " cout=" << ag->count;
      run = false;
    });
  }
  t << nothing;
}

}

BEGIN_SUITE(agmap, "")
  ADD_UNIT(agmap1)
END_SUITE(agmap)

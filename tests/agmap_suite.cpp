#include <fas/testing.hpp>
#include <wrtstat/manager/aggregator_map.hpp>
#include <wrtstat/manager/aggregator_hashmap.hpp>
#include <numeric>

namespace {
  

UNIT(agmap1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  aggregator_hashmap_options opt;
  opt.resolution = 1000000000;
  opt.soiled_start = 1000000000;
  opt.levels = 1;
  opt.limit  = 10;
  opt.step_ts = 500000000;
  opt.reduced_size = 13;
  
  aggregator_hashmap_mt agh(opt);
  
  reduced_data rd;
  bool run = true;
  while(run)
  {
    rd.ts=aggregator::now<std::chrono::nanoseconds>();
    rd.count = 1;
    agh.add("xxx", rd, [&run, &t]( aggregator_hashmap_mt::aggregated_ptr ag)
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

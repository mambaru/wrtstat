#include <iostream>
#include <wrtstat/wrtstat.hpp>

int main()
{
  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::none;
  opt.aggregation_step_ts = 10;
  wrtstat::wrtstat stat(opt);

  wrtstat::id_t id = stat.create_aggregator("latency", 0);
  auto meter = stat.create_value_meter(id);

  for (int i = 0; i < 100; ++i)
    meter.create(static_cast<wrtstat::value_type>(i), 1);

  if (auto ag = stat.force_pop(id))
  {
    std::cout << "count=" << ag->count
              << " min=" << ag->min
              << " perc50=" << ag->perc50
              << " perc100=" << ag->perc100
              << std::endl;
  }
  return 0;
}

#include <iostream>
#include <wrtstat/wrtstat.hpp>
#include <chrono>

int main()
{
  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::microseconds;
  opt.aggregation_step_ts = 1000000;
  wrtstat::wrtstat stat(opt);

  wrtstat::id_t id = stat.create_aggregator("payload_size", 0);
  auto meter = stat.create_size_meter(id);
  for (int i = 0; i < 50; ++i)
    meter.create(static_cast<wrtstat::value_type>(64 + i * 10));

  if (auto ag = stat.force_pop(id))
  {
    std::cout << "count=" << ag->count
              << " avg=" << ag->avg
              << " perc80=" << ag->perc80
              << std::endl;
  }
  return 0;
}

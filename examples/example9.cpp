#include <iostream>
#include <thread>
#include <wrtstat/wrtstat.hpp>
#include <chrono>

int main()
{
  wrtstat::wrtstat::options_type opt;
  opt.resolution = wrtstat::resolutions::microseconds;
  opt.aggregation_step_ts = 1000000;
  opt.handler = [](const std::string& name, wrtstat::aggregated_data::ptr ag)
  {
    std::cout << name
              << " count=" << ag->count
              << " perc50=" << ag->perc50
              << " perc100=" << ag->perc100
              << std::endl;
  };
  wrtstat::wrtstat stat(opt);

  wrtstat::id_t id = stat.create_aggregator("request_time", 0);
  auto meter = stat.create_time_meter<std::chrono::microseconds>(id);
  {
    auto point = meter.create(1);
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }

  stat.pushout();
  return 0;
}

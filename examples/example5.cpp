#include <iostream>
#include <wrtstat/wrtstat.hpp>
#include <unistd.h>
#include <iostream>
#include <chrono>

void test(std::function<void()>) 
{
  std::cout << "Привет!" << std::endl;
  sleep(1);
}

int main()
{
  wrtstat::wrtstat_mt mng;
  int id = mng.create_aggregator("my_name", 100000);
  auto meter_proto = mng.create_time_meter<std::chrono::nanoseconds>(id, std::time(0)*1000000, 100000);
  for (int i = 0; i < 5; ++i)
  {
    //auto handler = mng.create_handler<std::chrono::microseconds>(id, 10);
    auto meter = meter_proto->clone(std::time(0)*1000000, 1);
    test([meter](){});
  }
  
  if ( auto ag = mng.force_pop(id) )
  {
    std::cout << "size " << ag->data.size() << std::endl;
    std::cout << "count " << ag->count << std::endl;
    std::cout << "min " << ag->min << std::endl;
    std::cout << "perc50 " << ag->perc50 << std::endl;
    std::cout << "perc80 " << ag->perc80 << std::endl;
    std::cout << "perc95 " << ag->perc95 << std::endl;
    std::cout << "perc99 " << ag->perc99 << std::endl;
    std::cout << "perc100 " << ag->perc100 << std::endl;
  }
  
  return 0;
}

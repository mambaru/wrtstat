#include <wrtstat/manager/aggregator_map.hpp>
#include <wrtstat/manager/aggregator_hashmap.hpp>
#include <chrono>
#include <thread>
#include <vector>
using namespace wrtstat;

/*
aggregator_map rt;
aggregator_map rt2;
*/

typedef aggregator_hashmap aggregator_map_t;
//typedef aggregator_map aggregator_map_t;

aggregator_map_t rt;
aggregator_map_t rt2;

typedef aggregator_map_t::aggregated_ptr aggregated_ptr;
reduced_data rd;
std::mutex mutex;

std::vector<std::string> names;

void test();
void test()
{
  for (int k = 0 ; k < 1000 ; ++k)
  {
    for (int i = 0 ; i < 1000 ; ++i)
    {
      auto start = std::chrono::system_clock::now();
      int xcount = 1000;
      for (int x = 0 ; x < xcount ; ++x)
      for (int j = 0 ; j < 1000 ; ++j)
      {
        rd.ts = aggregator::now_t<std::chrono::microseconds>();
        const std::string& name = names[size_t(i*j)];
        rt.add(name, rd, [&name]( aggregated_ptr ag){
          rt2.add( name, *ag, [&name](aggregated_ptr ag1){
            std::cout << name << "=" << ag1->count << std::endl;
          });
          std::cout << "ready,";
          std::cout.flush();
        } );
      }
      auto finish = std::chrono::system_clock::now();
      auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
      std::cout << "span " << span << "mks rate " << xcount*1000l * 1000000l / ( span + 1) << "rps" << std::endl;
    }
  }
}

int main()
{
  size_t S = 4;

  aggregator_map_t::options_type opt;
  //opt.hash_size = 2048;
  opt.reducer_limit = 2048;
  opt.reducer_levels = 16;
  opt.outgoing_reduced_size = 1024;
  opt.aggregation_step_ts = 1000000;
  //opt.random_startup_offset = 1000000;
  opt.resolution = 1000000;
  opt.soiled_start_ts = 1000000;
  //opt.pool_size = 128;
  //opt.soiled_start = 0;
  rt = aggregator_map_t(opt);
  opt.aggregation_step_ts = 5000000;
  opt.soiled_start_ts = 5000000;
  rt2 = aggregator_map_t(opt);
  
  rd.data={1,2,3,4,5, 6,7,8,9,10};
  rd.count = rd.data.size();
  names.reserve(1000000);
  for (int i = 0 ; i < 1000000; ++i)
    names.push_back(std::to_string(i) + "bla-bla-bla");
  std::vector<std::thread> th;
  for (size_t i=0; i < S; ++i)
    th.push_back(std::thread(test));
  for (size_t i=0; i < S; ++i)
    th[i].join();
  return 0;
}

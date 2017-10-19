#include <wrtstat/aggregator_map.hpp>
#include <chrono>
#include <thread>
#include <vector>
using namespace wrtstat;

/*
aggregator_map rt;
aggregator_map rt2;
*/

typedef aggregator_hash aggregator_map_t;
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
  for(;;)
  for (int k = 0 ; k < 1000 ; ++k)
  {
    for (int i = 0 ; i < 1000 ; ++i)
    {
      auto start = std::chrono::system_clock::now();
      for (int x = 0 ; x < 1000 ; ++x)
      for (int j = 0 ; j < 1000 ; ++j)
      {
        rd.ts = aggregator::now<std::chrono::microseconds>();
        const std::string& name = names[size_t(i*j)];
        rt.add(name, rd, [&name]( aggregated_ptr ag){
          rt2.add( name, *ag, [&name](aggregated_ptr ag1){
            std::cout << name << "=" << ag1->count << std::endl;
          });
          //std::cout << "ready,";
          //std::cout.flush();
        } );
      }
      auto finish = std::chrono::system_clock::now();
      auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
      std::cout << "span " << span << "mks rate " << 1000*1000l * 1000000l / ( span + 1) << "rps" << std::endl;
    }
  }
}

int main()
{
  size_t S = 4;

  aggregator_map_t::options_type opt;
  opt.hash_size = 1024*1024;
  opt.limit = 1024;
  opt.levels = 16;
  opt.reduced_size = 1024;
  opt.step_ts = 1000000;
  opt.random_startup_offset = 1000000;
  rt = aggregator_map_t(opt);
  opt.step_ts = 5000000;
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

#include <wrtstat/wrtstat.hpp>
#include <chrono>
#include <thread>
#include <vector>
using namespace wrtstat;

wrtstat_st rt;
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
      for (int j = 0 ; j < 1000 ; ++j)
      {
        reduced_data rd;
        rd.ts = time(0);
        //std::string name = names[size_t(i*j)];
        //name.clear();
        rt.add(names[size_t(i*j)], rd );
        /*
        if ( auto h = rt.create_aggregator_handler( std::to_string(i+10000000000) + std::to_string(j+10000000000), time(0) ) )
        {
          h( reduced_data() );
        };
        */
      }
      auto finish = std::chrono::system_clock::now();
      auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
      std::cout << "span " << span << "mks rate " << 1000 * 1000000 / ( span + 1) << "rps" << std::endl;
    }
  }
}

int main()
{
  names.reserve(1000000);
  for (int i = 0 ; i < 1000000; ++i)
    names.push_back(std::to_string(i) + "bla-bla-bla");
  size_t S = 1;
  std::vector<std::thread> th;
  for (size_t i=0; i < S; ++i)
    th.push_back(std::thread(test));
  for (size_t i=0; i < S; ++i)
    th[i].join();
  return 0;
}

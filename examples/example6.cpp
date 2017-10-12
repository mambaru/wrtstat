#include <wrtstat/wrtstat.hpp>
#include <chrono>
using namespace wrtstat;

int main()
{
  wrtstat_mt rt;
  for (int k = 0 ; k < 1000 ; ++k)
  for (int i = 0 ; i < 1000 ; ++i)
  {
    auto start = std::chrono::system_clock::now();
    for (int j = 0 ; j < 1000 ; ++j)
    {
      if ( auto h = rt.create_aggregator_handler( std::to_string(i+10000000000) + std::to_string(j+10000000000), time(0) ) )
      {
        h( reduced_data() );
      };
    }
    auto finish = std::chrono::system_clock::now();
    auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
    std::cout << "span " << span << "mks rate " << 1000 * 1000000 / ( span + 1) << "rps" << std::endl;
  }
  return 0;
}

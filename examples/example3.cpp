#include <wrtstat/aggregator.hpp>
#include <wrtstat/manager/mutex/empty_mutex.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <unistd.h>

int main()
{
  ::srand( static_cast<unsigned int>(std::time(0)) );
  wrtstat::pool<wrtstat::empty_mutex> pool( 1000, 1000000 );
  wrtstat::aggregator_options opt;
  opt.aggregation_step_ts = 5;
  opt.outgoing_reduced_size = 100;
  opt.resolution = 0;
  opt.reducer_limit = 1000;
  opt.reducer_levels = 1000;
  wrtstat::aggregator ag( 0, opt, pool.get_allocator() );
  opt.aggregation_step_ts = 10;
  opt.reducer_limit = 100;
  opt.reducer_levels = 1;
  wrtstat::aggregator ag2( 0, opt);
  auto start = std::chrono::high_resolution_clock::now();
  for (int t = 0 ; t < 100; ++t)
  {
    auto s = std::chrono::system_clock::now();
    auto s2 = std::chrono::system_clock::now();
    auto f2 = s2;
    for (int i = 0 ; i < 100000; ++i)
    {
      auto span = std::chrono::duration_cast<std::chrono::nanoseconds>(f2 - s2).count();
      s2 = std::chrono::system_clock::now();
      if ( i != 0 ) ag.add( t, span, 1 );
      else ::usleep(10);
      f2 = std::chrono::system_clock::now();
    }
    auto f = std::chrono::system_clock::now();
    ag2.add( t, std::chrono::duration_cast<std::chrono::microseconds>(f - s).count(), 1);
  }
  
  auto finish = std::chrono::high_resolution_clock::now();
  auto span = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
  auto count = 100000*1001;
  while (auto d = ag.pop())
  {
    std::cout << "50%:"<< d->perc50 << " 80%:" << d->perc80 << " 99%:" << d->perc99<< " 100%:" << d->perc100 << " min: " << d->min << " max: " << d->max << std::endl; 
  }
  std::cout << "count: " << count << std::endl;
  std::cout << "time: " << span << std::endl;
  std::cout << "rate: " << std::size_t(count/(double(span)/1000000.0)) << std::endl;
  std::cout << "-------------------" << std::endl;
  while (auto d = ag2.pop())
  {
    std::cout <<   "50%:"  << d->perc50  << "(" << long(100000.0/(double(d->perc50)/1000000.0))
              << ") 80%:"  << d->perc80  << "(" << long(100000.0/(double(d->perc80)/1000000.0))
              << ") 100%:" << d->perc100 << "(" << long(100000.0/(double(d->perc100)/1000000.0))
              << ") min:" << d->min << "(" << long(100000.0/(double(d->min)/1000000.0))
              << ")" << std::endl; 
  }

  return 0;
}

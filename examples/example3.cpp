#include <wrtstat/aggregator.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <unistd.h>

int main()
{
  ::srand( std::time(0) );
  wrtstat::aggregator ag( 0, 5, 100000, 10, 1000000);
  wrtstat::aggregator ag2( 0, 10, 1000, 1, 1000);
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
      if ( i != 0 ) ag.add( t, span );
      else ::usleep(10);
      f2 = std::chrono::system_clock::now();
    }
    auto f = std::chrono::system_clock::now();
    ag2.add( t, std::chrono::duration_cast<std::chrono::microseconds>(f - s).count() );
  }
  ag.set_limit(100);
  auto finish = std::chrono::high_resolution_clock::now();
  auto span = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
  auto count = 100000*1001;
  while (auto d = ag.pop())
  {
    std::cout << "50%:"<< d->perc50 << " 80%:" << d->perc80 << " 99%:" << d->perc99<< " 100%:" << d->perc100 << " min: " << d->min << " max: " << d->max << std::endl; 
  }
  std::cout << "count: " << count << std::endl;
  std::cout << "time: " << span << std::endl;
  std::cout << "rate: " << std::size_t(count/(span/1000000.0)) << std::endl;
  std::cout << "-------------------" << std::endl;
  while (auto d = ag2.pop())
  {
    std::cout <<   "50%:"  << d->perc50  << "(" << long(100000.0/(d->perc50/1000000.0))
              << ") 80%:"  << d->perc80  << "(" << long(100000.0/(d->perc80/1000000.0))
              << ") 100%:" << d->perc100 << "(" << long(100000.0/(d->perc100/1000000.0))
              << ")" << std::endl; 
  }

  return 0;
}

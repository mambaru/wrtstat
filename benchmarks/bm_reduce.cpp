#include <iostream>
#include <wrtstat/reducer.hpp>
#include <wrtstat/manager/pool.hpp>
#include <wrtstat/manager/mutex/empty_mutex.hpp>
int main(int argc, char* argv[])
{
  using namespace wrtstat;
  if ( argc!=4 )
  {
    std::cout << "Usage:\n\t" << argv[0] << " limit levels counts" << std::endl;
    return -1;
  }
  reducer_options opt;
  opt.reducer_limit = static_cast<size_t>(atoi(argv[1]));
  opt.reducer_levels = static_cast<size_t>(atoi(argv[2]));
  size_t count = static_cast<size_t>(atoi(argv[3]));
  pool<empty_mutex> p(opt.reducer_limit*2, opt.reducer_levels*2);
  reducer r(opt, p.get_allocator());
  std::srand(1);
  
  std::cout << "start " << std::endl;
  auto start = std::chrono::steady_clock::now();
  
  for (size_t c = 0 ; c <count ; ++c)
  {
    for (size_t i = 0; i < opt.reducer_levels; ++i )
    {
      for (size_t j = 0; j < opt.reducer_limit; ++j )
      {
        r.add(std::rand()%10000, 1);
      }
    }
    r.detach();
  }
  auto finish = std::chrono::steady_clock::now();
  std::cout << "finish " << std::endl;
  auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
  std::cout << "span: " << span  << "mks " << span/1000 << "ms " << span/1000000 << "s" << std::endl;
  std::cout << "rate: " << count*1000000UL/size_t(span) << std::endl;
  /*
  for (size_t i = 0; i < opt.reducer_levels; ++i )
  {
    for (size_t j = 0; j < opt.reducer_limit; ++j )
    {
      r.add(std::rand()%10000, 1);
    }
  }
  std::vector< std::unique_ptr<reducer> > vr; 
  vr.reserve(count);
  for (size_t i = 0 ; i <count ; ++i)
  {
    vr.push_back(r.clone());
    //std::cout << vr.back()->size() << std::endl;
  }
    
  std::cout << "start " << std::endl;
  auto start = std::chrono::steady_clock::now();
  for (auto &pr : vr)
    pr->detach();
  auto finish = std::chrono::steady_clock::now();
  std::cout << "finish " << std::endl;
  auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
  std::cout << "span: " << span << std::endl;
  std::cout << "rate: " << count*1000000UL/size_t(span) << std::endl;
  */
  return 0;
}

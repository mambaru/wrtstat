#include <iostream>
#include <wrtstat/aggregator/aggregator.hpp>
#include <wrtstat/system/pool.hpp>

int main(int argc, char* argv[])
{
  using namespace wrtstat;
  if ( argc!=5 )
  {
    std::cout << "Usage:\n\t" << argv[0] << " limit levels mode counts" << std::endl;
    return -1;
  }
  aggregator_options opt;
  opt.reducer_limit = static_cast<size_t>(atoi(argv[1]));
  opt.reducer_levels = static_cast<size_t>(atoi(argv[2]));
  opt.reducer_mode = static_cast<reducer_options::mode>(atoi(argv[3]));
  opt.aggregation_step_ts = 5000000;
  size_t count = static_cast<size_t>(atoi(argv[4]));
  
  pool<std::mutex> p(opt.reducer_limit*2, opt.reducer_levels*2);
  aggregator r(aggregator::now(1000000), opt, p.get_allocator());
  std::srand(1);
  
  std::cout << "start " << std::endl;
  time_t span_min = std::numeric_limits<time_t>::max();
  auto start = std::chrono::steady_clock::now();
  
  for (size_t c = 0 ; c <count ; ++c)
  {
    auto start_once = std::chrono::steady_clock::now();
    for (size_t i = 0; i < opt.reducer_levels; ++i )
    {
      for (size_t j = 0; j < opt.reducer_limit; ++j )
      {
        r.add(aggregator::now(1000000), std::rand()%10000, 1);
      }
    }
    r.force_pop();
    auto finish_once = std::chrono::steady_clock::now();
    auto span_once = std::chrono::duration_cast<std::chrono::microseconds>( finish_once - start_once ).count();
    if ( span_once < span_min )
      span_min = span_once;
  }
  auto finish = std::chrono::steady_clock::now();
  std::cout << "finish " << std::endl;
  auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
  size_t span_mdl = size_t(span)/count; 
  std::cout << "span: " << span_mdl << "mks " << span_mdl/1000 << "ms " << span_mdl/1000000 << "s" << std::endl;
  std::cout << "rate: " << 1000000UL/span_mdl << std::endl;

  std::cout << "min_span: " << span_min  << "mks " << span_min/1000 << "ms " << span_min/1000000 << "s" << std::endl;
  std::cout << "max_rate: " << 1000000L/span_min << std::endl;

  return 0;
}

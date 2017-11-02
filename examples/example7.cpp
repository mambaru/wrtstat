#include <wrtstat/manager/aggregator_map.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
using namespace wrtstat;

int main()
{
  aggregator_map am;
  reduced_data rd;
  rd.ts = time(0);
  rd.data = {1,2,3,4,5,5};
  
  am.add("hello", rd, [](aggregator_map::aggregated_ptr){
    std::cout << "hello1" << std::endl;
  });
  
  sleep(1);
  rd.ts = time(0);
  am.add("hello", rd, [](aggregator_map::aggregated_ptr){
    std::cout << "hello2" << std::endl;
  });
  
  sleep(1);
  rd.ts = time(0);
  am.add("hello", rd, [](aggregator_map::aggregated_ptr){
    std::cout << "hello3" << std::endl;
  });
  
  return 0;
}

#include <wrtstat/multi_aggregator/basic_multi_aggregator.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
using namespace wrtstat;

int main()
{
  basic_multi_aggregator am;
  reduced_data rd;
  rd.ts = time(nullptr);
  rd.data = {1,2,3,4,5,5};
  
  am.push("hello", rd, [](request::push::ptr){
    std::cout << "hello1" << std::endl;
  });
  
  sleep(1);
  rd.ts = time(nullptr);
  am.push("hello", rd, [](request::push::ptr){
    std::cout << "hello2" << std::endl;
  });
  
  sleep(1);
  rd.ts = time(nullptr);
  am.push("hello", rd, [](request::push::ptr){
    std::cout << "hello3" << std::endl;
  });
  
  return 0;
}

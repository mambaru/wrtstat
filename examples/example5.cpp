#include <iostream>
#include <wrtstat/manager.hpp>
#include <unistd.h>
#include <iostream>

void test(std::function<void()>) 
{
  std::cout << "Привет!" << std::endl;
  sleep(1);
}

int main()
{
  wrtstat::manager mng;
  mng.reconfigure( wrtstat::manager::options_type() );
  int id = mng.reg_name("my_name", 100000);
  for (int i = 0; i < 5; ++i)
  {
    auto handler = mng.create_handler<std::chrono::microseconds>(id, 10);
    test([handler](){});
  }
  
  if ( auto ag = mng.force_pop(id) )
  {
    std::cout << "size " << ag->data.size() << std::endl;
    std::cout << "count " << ag->count << std::endl;
    std::cout << "min " << ag->min << std::endl;
    std::cout << "perc50 " << ag->perc50 << std::endl;
    std::cout << "perc80 " << ag->perc80 << std::endl;
    std::cout << "perc95 " << ag->perc95 << std::endl;
    std::cout << "perc99 " << ag->perc99 << std::endl;
    std::cout << "perc100 " << ag->perc100 << std::endl;
  }
  
  return 0;
}

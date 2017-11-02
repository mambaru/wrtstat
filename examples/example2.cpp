#include <iostream>
#include <array>
#include <algorithm>
#include <wrtstat/reducer.hpp>
#include <wrtstat/separator.hpp>
#define LEVEL1 10
#define LEVEL2 1
int main()
{
  {
    std::array<int, LEVEL1> values;
    
    for (int i = 0; i < LEVEL2; ++i)
    {
      int pos = i;
      for (int j = 0; j < LEVEL1; ++j)
      {
        if (pos >= LEVEL1)
           break;
        values[ size_t(pos) ] = i+1;
        pos += (i+1);
      }
    }
    std::cout << values.size() << std::endl;
    for ( int i : values )
      std::cout << i << " ";
    std::cout << std::endl;
    
    for (int i = 1; i < LEVEL2+1; ++i)
      std::cout << i << " = " << std::count(values.begin(),values.end(), i) << std::endl;
    
  }
  std::cout << "-------------------------------------" << std::endl;
  {
    wrtstat::reducer_options opt;
    opt.reducer_limit = LEVEL1; opt.reducer_levels = LEVEL2;
    wrtstat::reducer c(opt);
    c.add(100, 1); c.add(101, 1); c.add(102, 1);
    for (int i = 0; i < LEVEL2; ++i)
    {
      for (int j = 0; j < LEVEL1; ++j)
        c.add(i+1, 1);
    }
    
    std::cout << "lossy_count: " << c.lossy_count() << std::endl;
    auto d = c.detach();
    for ( int i : d->data )
      std::cout << i << " ";
    std::cout << std::endl;
    for (int i = 1; i < LEVEL2+1; ++i)
      std::cout << i << " = " << std::count(d->data.begin(), d->data.end(), i) << std::endl;
    
  }
  return 0;
}

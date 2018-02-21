#include <iostream>
#include <array>
#include <algorithm>
//#include <wrtstat/aggregator.hpp>
#define LEVEL1 100
#define LEVEL2 50
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
        values[ std::size_t(pos) ] = i+1;
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
  return 0;
}

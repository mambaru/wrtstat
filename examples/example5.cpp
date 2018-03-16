#include <chrono>
#include <unordered_map>
#include <unordered_map>
#include <iostream>
const int SIZE = 1000000;

int main()
{
  std::unordered_map<int, int> m;
  m.reserve(SIZE);
  long c = 0;
  time_t t = std::numeric_limits<time_t>::max();
  for (int k = 0; k < 10; ++k)
  {
    m.clear();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < SIZE;++i)
    {
      m.insert(std::make_pair(i*i, i));
      c+=i;
      //c += m[i*i] += i;
    }
    auto finish = std::chrono::high_resolution_clock::now();
    time_t span = std::chrono::duration_cast<std::chrono::milliseconds>( finish - start).count();
    if ( span < t )
      t = span;
    std::cout << t << "ms" << std::endl;
  }
  
  std::cout << "-------" << std::endl;  
  std::cout << c << std::endl;
  std::cout << t << "ms" << std::endl;

}

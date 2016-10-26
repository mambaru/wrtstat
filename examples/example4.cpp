#include <cstddef>
#include <iostream>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <wrtstat/aggregator_map.hpp>


std::string operator "" _json(const char* str, size_t size)
{
  std::string ss( str, str + size);
  std::replace( ss.begin(), ss.end(), '\'', '\"');
  return ss;
}

int main()
{
  std::cout << "{'method':'plus','params':{'first':2,'second':3},'id':1}"_json << std::endl;
}

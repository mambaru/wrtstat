#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include <ctime>

namespace wrtstat {

struct types
{
  typedef long long int value_type;
  typedef std::vector<value_type> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::size_t size_type;
  typedef std::time_t time_type;
};

}

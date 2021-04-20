#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include <ctime>

namespace wrtstat {

typedef std::size_t size_type;
typedef std::time_t time_type;
typedef std::time_t value_type;
typedef std::size_t id_t;
typedef std::vector<value_type> data_type;
typedef std::unique_ptr<data_type> data_ptr;

constexpr id_t bad_id = static_cast<id_t>(-1);
constexpr id_t get_bad_id() { return bad_id; }

enum class resolutions
{
  none = 0,
  seconds = 1, 
  milliseconds = 1000,
  microseconds = 1000000,
  nanoseconds = 1000000000
};

}

#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <ctime>
#include <functional>

namespace wrtstat {

typedef int64_t value_type;
typedef std::vector<value_type> data_type;
typedef std::unique_ptr<data_type> data_ptr;
typedef std::size_t size_type;
typedef std::time_t time_type;
typedef std::size_t id_t;
static const id_t bad_id = static_cast<id_t>(-1);

}

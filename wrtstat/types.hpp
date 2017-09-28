#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <ctime>
#include <functional>

namespace wrtstat {

struct types
{
  typedef long long int value_type;
  typedef std::vector<value_type> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::size_t size_type;
  typedef std::time_t time_type;
  typedef std::time_t span_type;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<mutex_type> mutex_ptr;
  typedef std::weak_ptr<mutex_type> mutex_wptr;
  typedef std::function< void(time_type now, span_type value, size_type count) > meter_fun_t;
  typedef std::function< void(time_type now, data_type&& value, size_type count) > handler_fun_t;
  
};

}

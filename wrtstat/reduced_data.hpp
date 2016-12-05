#pragma once
#include <cstddef>
#include <vector>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct reduced_data
{
  typedef types::value_type value_type;
  typedef types::data_type data_type;
  typedef types::size_type size_type; 
 
  // Общее количество
  size_type count = 0;
  // Количество не учтенных
  size_type lossy = 0;
  // min - это не 0% с потярми
  value_type min = 0;
  // Ахтунг! Если есть потери, то max-это не 100%
  value_type max = 0;
  // Среднее считаем здесь, для точности
  value_type avg = 0;
  data_type data;
};

}

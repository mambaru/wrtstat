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
  size_type count;
  // Количество не учтенных
  size_type lossy;
  // min - это не 0% с потярми
  value_type min;
  // Ахтунг! Если есть потери, то max-это не 100%
  value_type max;
  // Среднее считаем здесь, для точности
  value_type avg;
  data_type data;
};

}

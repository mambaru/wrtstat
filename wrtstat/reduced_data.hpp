#pragma once
#include <cstddef>
#include <vector>

namespace wrtstat {

struct reduced_data
{
  typedef long long int value_type;
  typedef std::vector<value_type> data_type;
 
  // Общее количество
  size_t count;
  // Количество не учтенных
  size_t lossy;
  // min - это не 0% с потярми
  value_type min;
  // Ахтунг! Если есть потери, то max-это не 100%
  value_type max;
  // Среднее считаем здесь, для точности
  value_type avg;
  data_type data;
};

}

#pragma once
#include <cstddef>
#include <vector>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct reduced_info
{
  time_type ts = 0;
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
  
  reduced_info& operator += (const reduced_info& value);
};

inline reduced_info& reduced_info::operator += (const reduced_info& value)
{
  if ( this->count+value.count != 0)
    this->avg = ( value_type(this->count)*this->avg + value_type(value.count)*value.avg) / value_type(this->count+value.count);
  else
    this->avg = (this->avg + value.avg) / 2;
  this->count += value.count;
  this->lossy += value.lossy;
  this->min = std::min(this->min, value.min);
  this->max = std::min(this->max, value.max);
  
  return *this;
}
  
struct reduced_data: reduced_info
{
  data_type data;
};

}

//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>

namespace wrtstat {

struct packer_options
{
  size_t push_limit = 0;
  size_t data_limit = 0; 
  size_t json_limit = 0;
  bool json_compact = false;
  
  std::string name_sep = "~~";
  bool name_compact = false;
  // Для отладки. Сериализация в JSON и сравнение с расчетными (0 - отключен, всегда допускаем хотябы 1 - вкл)
  size_t debug_json_push_gap = 0; // для каждого пакета
  size_t debug_json_result_gap = 0; // для результата

};

}

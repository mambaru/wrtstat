#pragma once
#include <wrtstat/reducer_options.hpp>
#include <wrtstat/types.hpp>

namespace wrtstat {

struct separator_options
  : reducer_options
{
  // шаг обрезания. Если нужен шаг обрезания 5 сек для микросекунд (resolution=1000000), то нужно указать 5000000
  types::time_type step_ts = 1;
  // resolution 1, 1000, 1000000, 1000000000
  types::time_type resolution = 0;
  // смещает начальное значение для того чтобы аггрегация не шла лавиной
  // значение аналогично step_ts, обычно достаточно чтобы было равным step_ts
  types::time_type soiled_start = 0;
};

}

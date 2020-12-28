//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wrtstat/api/push.hpp>
#include <deque>

namespace wrtstat{

namespace request
{
  struct multi_push
  {
    std::string sep="~~";
    //При непустом legends предполагаеться преобразоание имен типа name~~name1~~name2~~name3
    // в имена 0~1~2~3, где каждое число индекс массива legends[name,name1,name2,name3]
    std::vector<std::string> legend;
    std::deque<push> data;
    typedef std::unique_ptr<multi_push> ptr;
  };
}

namespace response
{
  struct multi_push
  {
    // false - нет места для какого-то нового счетчика
    bool status = true;
    std::string error;
    typedef std::unique_ptr<multi_push> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}

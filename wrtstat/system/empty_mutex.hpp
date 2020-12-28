//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

namespace wrtstat{
  
class empty_mutex
{

public:
  empty_mutex(){}

  empty_mutex(const empty_mutex&) = delete;
  empty_mutex& operator=(const empty_mutex&) = delete;

  static inline void lock(){}
    
  static inline void unlock(){}

  static inline void lock_r() {}
  
  static inline void unlock_r() {}
};

  
}

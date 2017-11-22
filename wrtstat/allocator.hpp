#pragma once

#include <wrtstat/types.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace wrtstat {

  class allocator
  {
  public:
    typedef std::function<data_ptr()> create_handle;
    typedef std::function<data_ptr(data_ptr)> free_handle;

    allocator(){}
    allocator(const create_handle& c, const free_handle& f)
      : _create(c) 
      , _free(f)
    {}

    data_ptr create() 
    {
      if ( _create!=nullptr)
        return _create();
      return data_ptr(new data_type);
    }

    data_ptr free( data_ptr d ) 
    { 
      if ( _free!=nullptr )
        return _free(std::move(d));
      return d;
    }
  private:
    create_handle _create;
    free_handle _free;
  };


}

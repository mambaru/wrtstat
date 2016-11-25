#pragma once
#include <wrtstat/types.hpp>

namespace wrtstat {

class pool
{
public:
  typedef types::size_type size_type;
  typedef types::data_type data_type;
  typedef types::data_ptr  data_ptr;
  typedef std::vector<data_ptr> data_pool;
public:
  class allocator
  {
  public:
    allocator(): _p(nullptr) {}
    allocator(pool* p): _p(p) {}
    data_ptr create() 
    {
      if ( _p!=nullptr)
        return _p->create();
      return data_ptr(new data_type);
    }
    data_ptr free( data_ptr d ) 
    { 
      if ( _p!=nullptr )
        return _p->free(std::move(d));
      return std::move(d);
    }
  private:
    pool* _p ;
  };
  
  allocator get_allocator() { return allocator(this);}
  
  pool(size_type item_size, size_type pool_size)
    : _item_size( item_size )
    , _pool_size(pool_size)
  {}
  
  data_ptr create()
  {
    data_ptr d;
    if ( _pool.empty() )
    {
      d = data_ptr(new data_type);
      d->reserve(_item_size);
    }
    else
    {
      d = std::move(_pool.back());
      _pool.pop_back();
    }
    if ( d == nullptr )
      abort();
    return std::move(d);
  }
  
  data_ptr free( data_ptr d )
  {
    if ( d == nullptr )
      return nullptr;

    if ( _pool.size() == _pool_size )
      return std::move(d);

    if ( d->capacity() > _item_size )
    {
      d->resize(_item_size);
      d->shrink_to_fit();
    }
    else
      d->reserve(_item_size);
    
    d->clear();
    
    _pool.push_back( std::move(d) );
    return nullptr;
  }
  
private:
  size_type _item_size;
  size_type _pool_size;
  data_pool _pool;
};

}

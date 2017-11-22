#pragma once
#include <wrtstat/types.hpp>
#include <wrtstat/allocator.hpp>
#include <vector>

namespace wrtstat {

class pool
{
public:
  typedef std::vector<data_ptr> data_pool;
  
public:
  pool(size_type item_size, size_type pool_size)
    : _item_size( item_size )
    , _pool_size(pool_size)
  {}

  allocator get_allocator()
  { 
    return allocator(
      std::bind(&pool::create, this),
      std::bind(&pool::free, this, std::placeholders::_1)
    );
  }

  pool(const pool&) = delete;
  pool& operator=(const pool&) = delete;
  
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
    return d;
  }
  
  data_ptr free( data_ptr d )
  {
    if ( d == nullptr )
      return nullptr;

    if ( _pool.size() == _pool_size )
      return d;

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

/*
class pool_mt: private pool
{
public:
  typedef pool::size_type size_type;
  typedef pool::data_type data_type;
  typedef pool::data_ptr  data_ptr;

  pool_mt(size_type item_size, size_type pool_size)
    : pool( item_size , pool_size)
  {}

  allocator get_allocator()
  { 
    return allocator(
      std::bind(&pool_mt::create, this),
      std::bind(&pool_mt::free, this, std::placeholders::_1)
    );
  }

  data_ptr create()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return pool::create();
  }
  
  data_ptr free( data_ptr d )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return pool::free( std::move(d));
  }
private:
  mutable std::mutex _mutex;
};
*/
}

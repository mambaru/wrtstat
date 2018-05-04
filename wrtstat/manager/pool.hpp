#pragma once
#include <wrtstat/types.hpp>
#include <wrtstat/allocator.hpp>
#include <wrtstat/manager/mutex/rwlock.hpp>
#include <vector>
#include <deque>
#include <mutex>
#include <iostream>

namespace wrtstat {

template<typename MutexType>
class pool
{
public:
  typedef std::deque<data_ptr> data_pool;
  typedef rwlock<MutexType> mutex_type;
  
public:
  pool(size_type item_size, size_type pool_size)
    : _item_size(item_size)
    , _pool_size(pool_size)
  {
    //_pool.reserve(pool_size);
  }

  allocator get_allocator()
  { 
    return allocator(
      std::bind(&pool::create, this),
      std::bind(&pool::free, this, std::placeholders::_1)
    );
  }

  pool(const pool&) = delete;
  pool& operator=(const pool&) = delete;
  
  bool empty() const
  {
    read_lock<mutex_type> lk(_mutex);
    return _pool.empty();
  }

  bool filled() const
  {
    read_lock<mutex_type> lk(_mutex);
    return _pool.size() == _pool_size;
  }

  
  data_ptr create()
  {
    data_ptr d;
    if ( this->empty() )
    {
      d = data_ptr(new data_type);
      d->resize(_item_size, value_type() );
      d->clear();
    }
    else
    {
      std::lock_guard<mutex_type> lk(_mutex);
      d = std::move(_pool.back());
      _pool.pop_back();
    }
    return d;
  }
  
  data_ptr free( data_ptr d )
  {
    if ( d == nullptr )
      return nullptr;

    if ( this->filled() )
      return d;

    if ( d->capacity() > _item_size )
    {
      d->resize(_item_size);
      d->shrink_to_fit();
    }
    else
      d->resize(_item_size, value_type());

    d->clear();

    std::lock_guard<mutex_type> lk(_mutex);
    _pool.push_back( std::move(d) );
    return nullptr;
  }

private:
  size_type _item_size;
  size_type _pool_size;
  data_pool _pool;
  mutable mutex_type _mutex;
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

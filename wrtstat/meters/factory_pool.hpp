#pragma once

#include <wrtstat/types.hpp>
#include <wrtstat/aggregator.hpp>
#include <chrono>
#include <memory>
#include <iostream>
#include <vector>

#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>



namespace wrtstat {

  
template <typename T, typename Pool = boost::object_pool<T> >
struct pool_deleter {
    pool_deleter(Pool& pool) : _pool(pool) {}
    void operator()(T*p) const { _pool.destroy(p); }
  private:
    Pool& _pool;
};

template<typename T>
class factory_pool
  : public std::enable_shared_from_this<factory_pool<T>>
{
public:
  typedef T  value_type;
  typedef T* raw_ptr;
  typedef std::shared_ptr<T> ptr;
  typedef std::mutex mutex_type;
  typedef std::weak_ptr< factory_pool<T> > wthis_ptr;
  

  
  explicit factory_pool(size_t size)
    : _size(size)
  {
    //_pool.reserve(size);
  };
  
  ~factory_pool()
  {
    /*for (auto p : _pool)
      delete p;*/
    //_pool.clear();
    
  }
  
  factory_pool(const factory_pool&) = delete;
  factory_pool(factory_pool&&) = default;
  
  factory_pool& operator=(const factory_pool&) = delete;
  factory_pool& operator=(factory_pool&&) = default;
  
  
  template<class... Args>
  ptr make(Args&&... args)
  {
    using namespace std::placeholders;
    auto res = std::allocate_shared<T>( boost::pool_allocator<T>()/*, std::forward<Args>(args)...*/ );
    res->reset(std::forward<Args>(args)...);
    return res;
    /*
    //raw_ptr p = nullptr;
    ptr p = nullptr;
    {
      std::lock_guard<std::mutex> lk(_m);
      if ( _pool.empty() || _pool.size() > _size )
      {
        //p = new T(std::forward<Args>(args)...);
        raw_ptr rp = new T(std::forward<Args>(args)...);
        p = ptr(rp, std::bind(&factory_pool<T>::release, this->shared_from_this(), _1));
      }
      else
      {
        p = _pool.back();
        _pool.pop_back();
        p->reset(std::forward<Args>(args)...);
      }
    }
    */
    /*wthis_ptr wthis = this->shared_from_this();
    return ptr(p, std::bind(&factory_pool<T>::release, wthis, _1) );
    */
    //return p;
  }
  
  static void release(wthis_ptr wthis, raw_ptr p)
  {

    using namespace std::placeholders;
    if ( auto pthis = wthis.lock() )
    {
      std::lock_guard<std::mutex> lk(pthis->_m);
      if ( pthis->_pool.size() < pthis->_size ) 
      {
        //pthis->_pool.push_back( p);
        pthis->_pool.push_back( ptr(p, std::bind(&factory_pool<T>::release, wthis, _1) ));
      }
      else
        delete p;
    }
  }
private:
  size_t _size;
  std::vector</*raw_*/ptr> _pool_old;
  ::boost::object_pool<ptr> _pool;
  mutex_type _m;
};

}

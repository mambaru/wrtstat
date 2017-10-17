#pragma once
#include <wrtstat/manager/mutex/rwlock.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>

namespace wrtstat {

template<typename Mutex>
class dict
{
  typedef rwlock<Mutex> mutex_type;
  
public:
  
  typedef std::size_t id_t;
  
  dict(id_t initial, id_t step)
    : _counter(initial)
    , _step(step)
  {
  }

  id_t create_id(const std::string& name)
  {
    {
      read_lock<mutex_type> lk(_mutex);
      auto itr = _dict.find(name);
      if ( itr != _dict.end() )
        return itr->second;
    }

    std::lock_guard<mutex_type> lk(_mutex);
    id_t id = 0;
    if ( _free.empty() ) 
    {
      _counter += _step;
      id = _counter;
    }
    else
    {
      id = *(_free.begin());
      _free.erase(_free.begin());
    }
    _dict.insert( std::make_pair(name, id) );
    _index.insert( std::make_pair(id, name) );
    return id;
  }

  std::string get_name(id_t id) const
  {
    read_lock<mutex_type> lk(_mutex);
    auto itr = _index.find(id);
    if ( itr == _index.end() )
      return std::string();
    return itr->second;
  }
  
  id_t get_id(const std::string& name) const
  {
    read_lock<mutex_type> lk(_mutex);
    auto itr = _dict.find(name);
    if ( itr == _dict.end() )
      return static_cast<id_t>(-1);
    return itr->second;
  }
  
  bool free(id_t index) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _index.find(index);
    if ( itr == _index.end() )
      return false;
    
    _free.insert( itr->first );
    _dict.erase( itr->second );
    _index.erase( itr );
    return false;
  }
  
  bool free(const std::string& name)
  {
    id_t id = this->get_id(name);
    if ( id == static_cast<id_t>(-1) )
      return false;
    return this->free(id);
  }
  
  void clear()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _counter = 0;
    _dict.clear();
    _index.clear();
    _free.clear();
  }
  
  size_t id2pos(id_t id) const
  {
    return (id - _step) / _step;
  }

private:
  mutable mutex_type _mutex;
  id_t _counter = 0;
  const id_t _step = 1;
  std::unordered_map< std::string, id_t> _dict;
  std::unordered_map<id_t, std::string> _index;
  std::unordered_set<id_t> _free;
};
}

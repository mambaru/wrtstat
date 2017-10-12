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

  int create_id(const std::string& name)
  {
    {
      read_lock<mutex_type> lk(_mutex);
      auto itr = _dict.find(name);
      if ( itr != _dict.end() )
        return itr->second;
    }

    std::lock_guard<mutex_type> lk(_mutex);
    int id = 0;
    if ( _free.empty() ) 
    {
      id = _counter++;
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

  std::string get_name(int id) const
  {
    read_lock<mutex_type> lk(_mutex);
    auto itr = _index.find(id);
    if ( itr == _index.end() )
      return std::string();
    return itr->second;
  }
  
  int get_id(const std::string& name) const
  {
    read_lock<mutex_type> lk(_mutex);
    return this->get_id_(name);
  }
  
  bool free(int index) 
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
    int id = this->get_id(name);
    if ( id == -1 )
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
  
private:

  int get_id_(const std::string& name) const
  {
    auto itr = _dict.find(name);
    if ( itr == _dict.end() )
      return -1;
    return itr->second;
  }

private:
  mutable mutex_type _mutex;
  int _counter = 1;
  std::unordered_map< std::string, int> _dict;
  std::unordered_map<int, std::string> _index;
  std::unordered_set<int> _free;
};
}

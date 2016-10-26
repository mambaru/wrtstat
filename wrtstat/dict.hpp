#pragma once
#include <map>
#include <set>
#include <string>
#include <memory>

namespace wrtstat {

class dict
{
  typedef std::shared_ptr<std::string> string_ptr;
  struct less_ptr
  {
    bool operator()(string_ptr first, string_ptr second) const
    {
      return *first < *second;
    }
  };
public:
  int get(std::string name)
  {
    auto pname = std::make_shared<std::string>( std::move(name) ); 
    auto itr = _dict.find(pname);
    if ( itr != _dict.end() )
      return itr->second;

    int value = 0;
    if ( _free.empty() ) value = _counter++;
    else
    {
      value = *(_free.begin());
      _free.erase(_free.begin());
    }
    _dict.insert( std::make_pair(pname, value) );
    _index.insert( std::make_pair(value, pname) );
  }

  void free(int index) 
  {
    auto itr = _index.find(index);
    if ( itr == _index.end() )
      return;
    
    _free.insert( itr->first );
    _dict.erase( itr->second );
    _index.erase( itr );
  }
  
  void clear()
  {
    _counter = 0;
    _dict.clear();
    _index.clear();
    _free.clear();
  }
  
private:
  int _counter = 1;
  std::map<string_ptr, int, less_ptr> _dict;
  std::map<int, string_ptr> _index;
  std::set<int> _free;
};
}
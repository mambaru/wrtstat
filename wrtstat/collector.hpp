#pragma once
#include <vector>
#include <limits>
#include <memory>
#include <algorithm>

namespace wrtstat {

class collector
{
public:
  typedef long long int value_type;
  typedef std::vector<value_type> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::vector<data_ptr> data_list;
  

public:

  collector(size_t limit, size_t levels, size_t reserve=0)
    : _limit( limit )
    , _levels(levels)
    , _reserve( reserve )
  {
    if ( _reserve == 0 ) 
      _reserve = _limit;
  }

  size_t lossy_count() const 
  { 
    if ( _data.empty() )
      return 0;
    
    if ( _data.size() == 1 )
      return _lossy_count;

    return _lossy_count + _limit * (_data.size() - 2) + _data.back()->size();  

  }
  size_t total_count() const { return _total_count; }
  size_t levels() const { return _levels; }
  size_t max() const { return _max; }
  size_t min() const { return _min; }
  size_t position() const { return _position; }
  size_t size() const { return _data.size(); }

  bool filled() const { return _data.size()==_levels && _data.back()->size()==_limit;}
  
  void clear()
  {
    _min = std::numeric_limits<value_type>::max();
    _max = std::numeric_limits<value_type>::max();
    _lossy_count = 0;
    _total_count = 0;
    _position = 0;
    _data.clear();
  }

  void minmax(value_type v)
  {
    _min = std::min(_min, v);
    _max = std::max(_max, v);
  }

  void add( const data_type& values, size_t count)
  {
    for (value_type v : values)
      this->add(v);
    
    if ( values.size() != count )
    {
      auto diff = count - values.size();
      _lossy_count += diff;
      _total_count += diff;
    }
  }

  void add( const data_type& values)
  {
    this->add( values, values.size() );
  }

  void add( std::initializer_list<value_type> values )
  {
    for (value_type v : values)
      this->add(v);
  }

  void add( value_type v) 
  {
    this->add_(v);
  }
  
  data_ptr detach()
  {
    if (_data.empty() )
      return nullptr;
    this->reduce();
    auto res = std::move(_data.front());
    _data.front()=std::unique_ptr<data_type>();
    return std::move(res);
    /*
    size_t ds = _data[0]->size();
    for ( size_t i = 0, l = _data.size(); i < ds; ++i )
    {
      if ( l == _data.size() ) l=1;
      else if ( i < _data[l]->size() )
        _data[0]->at(i)=_data[l++]->at(i);
      else if (_data.size() > 2 )
        _data[0]->at(i) = _data[(l++)-1]->at(i);
    }
    auto res = std::move( _data[0] );
    _data.clear();
    std::sort( res->begin(), res->end() );
    return res;
    */
  }

  void reduce()
  {
    if ( _data.empty() )
      return;

    //if ( _data.back()->size() != _limit )
      std::sort( _data.back()->begin(), _data.back()->end() );
    
    if ( _data.size() == 1 )
      return;

    for ( size_t i = 1, l = 1; i < _limit; ++i )
    {
      if ( l == _data.size() ) // первую строку не трогаем 
        l=1;
      else if ( i < _data[l]->size() ) // незаполненный 
        _data[0]->at(i)=_data[l++]->at(i);
      else if (_data.size() > 2 )
        _data[0]->at(i) = _data[(l++)-1]->at(i);
    }
    _data.resize(1);
    std::sort( _data.front()->begin(), _data.front()->end() );
  }
private:
  

  void add_( value_type v) 
  {
    ++_total_count;
    this->minmax(v);
    
    if ( _data.empty() || _data.back()->size() == _limit  )
    {
      if ( _data.size() == _levels )
      {
        ++_lossy_count;
        return;
      }
      if ( !_data.empty() )
        std::sort( _data.back()->begin(), _data.back()->end() );
      _data.emplace_back( std::unique_ptr<data_type>(new data_type) );
    }
    _data.back()->push_back(v);
  }

private:
  
  value_type _min = std::numeric_limits<value_type>::max();
  value_type _max = std::numeric_limits<value_type>::max();
  // Счетчик отброшенных после заполнения
  size_t _lossy_count = 0;
  size_t _total_count = 0;
  // Ограничение на размер данных
  size_t _limit = 0;
  // Текущий уровень фильтрации
  size_t _levels = 0;
  size_t _reserve = 0;
  //
  size_t _position = 0;
  // Значения
  data_list _data;
};


}

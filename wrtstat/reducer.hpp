#pragma once
#include <wrtstat/reduced_data.hpp>
#include <wrtstat/reducer_options.hpp>
#include <wrtstat/manager/pool.hpp>
#include <vector>
#include <limits>
#include <memory>
#include <algorithm>


namespace wrtstat {

class reducer
{
public:
  typedef types::size_type size_type;
  typedef reduced_data::value_type value_type;
  typedef reduced_data::data_type data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::vector<data_ptr> data_list;

  typedef reduced_data reduced_type;
  typedef std::unique_ptr<reduced_type> reduced_ptr;

public:

  reducer(reducer_options opt, pool::allocator allocator = pool::allocator() )
    : _opt( opt )
    , _allocator( allocator )
  {
  }

  size_t lossy_count() const 
  { 
    if ( _data.empty() )
      return 0;
    if ( _data.size() == 1 )
      return _lossy_count;
    return _lossy_count + _opt.limit * (_data.size() - 2) + _data.back()->size();  
  }

  size_t total_count() const { return _total_count; }
  size_t levels() const { return _opt.levels; }
  size_t max() const { return _max; }
  size_t min() const { return _min; }
  
  size_t size() const 
  { 
    size_t result = 0;
    for ( auto& p : _data )
      if ( p!=nullptr )
        result += p->size();
    return result; 
    //return _data.size(); 
    
  }

  bool filled() const { return _data.size()==_opt.levels && _data.back()->size()==_opt.limit;}
  
  void clear()
  {
    //std::lock_guard<mutex_type> lk(tmp_mutex);
    
    _min = std::numeric_limits<value_type>::max();
    _max = std::numeric_limits<value_type>::min();
    _average = 0;
    _lossy_count = 0;
    _total_count = 0;
//    _position = 0;
    _data.clear();
  }

  void minmax(value_type v)
  {
    _min = std::min(_min, v);
    _max = std::max(_max, v);
  }

  void add( const data_type& values, size_t count)
  {
    //std::lock_guard<mutex_type> lk(tmp_mutex);
    for (value_type v : values)
      this->add_(v);
    
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
    //std::lock_guard<mutex_type> lk(tmp_mutex);
    for (value_type v : values)
      this->add_(v);
  }

  void add( value_type v, size_t count) 
  {
    // std::lock_guard<mutex_type> lk(tmp_mutex);
    this->add_(v);
    if ( count > 1 )
    {
      --count;
      _lossy_count += count;
      _total_count += count;
    }
  }
  
  reduced_ptr detach()
  {
    //std::lock_guard<mutex_type> lk(tmp_mutex);
    if ( this->empty() )
      return nullptr;

    auto res = reduced_ptr(new reduced_type);
    this->reduce();
    _data.front()->swap(res->data);
    res->avg = _average;
    res->count = _total_count;
    res->lossy = _lossy_count;
    res->max = _max;
    res->min = _min;
    this->clear();
    return std::move(res);
  }
  

  bool empty()
  {
    return _data.empty() && _total_count == 0;
  }

  void reduce()
  {
    if ( _data.empty() )
      return;

    std::sort( _data.back()->begin(), _data.back()->end() );

    if ( _data.size() == 1 )
      return;

    // Расчетное становится реальным
    _lossy_count = this->lossy_count();

    for ( size_t i = 1, l = 1; i < _opt.limit; ++i )
    {
      if ( l == _data.size() ) // первую строку не трогаем 
        l=1;
      else if ( i < _data[l]->size() ) // незаполненный 
        _data[0]->at(i)=_data[l++]->at(i);
      else if (_data.size() > 2 )
        _data[0]->at(i) = _data[(l++)-1]->at(i);
    }
    for ( size_t i = 1; i < _data.size(); ++i)
      _allocator.free( std::move(_data[i]) );
    _data.resize(1);
  }
private:
  

  void add_( value_type v) 
  {
    ++_total_count;
    _average += v;
    _average /= 2;

    this->minmax(v);
    
    if ( _data.empty() || _data.back()->size() == _opt.limit  )
    {
      if ( _data.size() == _opt.levels )
      {
        ++_lossy_count;
        return;
      }
      if ( !_data.empty() )
        std::sort( _data.back()->begin(), _data.back()->end() );
      _data.push_back( _allocator.create() );
    }

    _data.back()->push_back(v);
  }

private:
  const reducer_options _opt;
  value_type _min = std::numeric_limits<value_type>::max();
  value_type _max = std::numeric_limits<value_type>::min();
  // Счетчик отброшенных после заполнения
  size_t _lossy_count = 0;
  size_t _total_count = 0;
  // TODO: убрать и сделать аллкатор 
  pool::allocator _allocator;
  value_type _average = 0;
  //
  //size_t _position = 0;
  // Значения
  data_list _data;
};


}

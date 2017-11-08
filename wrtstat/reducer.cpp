#include <wrtstat/reducer.hpp>


namespace wrtstat {
  
reducer::reducer(const reducer_options& opt, const allocator& a  )
  : _opt( opt )
  , _allocator( a )
{}
  
size_t reducer::lossy_count() const 
{ 
  if ( _data.size() < 2 )
    return _lossy_count;
  return _lossy_count + _opt.reducer_limit*( _data.size() - 2 ) + _data.back()->size();  
}
  
size_t reducer::total_count() const 
{
  return _total_count; 
}
  
size_t reducer::levels() const 
{
  return _opt.reducer_levels; 
}
  
value_type reducer::max() const 
{ 
  return _max == std::numeric_limits<value_type>::min() ? 0 : _max; 
}
  
value_type reducer::min() const 
{ 
  return _min == std::numeric_limits<value_type>::max() ? 0 : _min; 
}
  
size_t reducer::size() const 
{ 
  size_t result = 0;
  for ( auto& p : _data )
  {
    if ( p!=nullptr )
      result += p->size();
  }
  return result; 
}
  
bool reducer::filled() const 
{
  if ( _opt.reducer_levels == 0 || _opt.reducer_limit==0)
    return true;
   
  if ( _data.empty() )
    return false;
  
  return _data.size()==_opt.reducer_levels && _data.back()->size()==_opt.reducer_limit;
}
  
void reducer::clear()
{
  _empty = true;
  _min = std::numeric_limits<value_type>::max();
  _max = std::numeric_limits<value_type>::min();
  _average = 0.0;
  _average_count = 0;
  _lossy_count = 0;
  _total_count = 0;
  _data.clear();
}
  
void reducer::minmax(value_type v)
{
  _min = std::min(_min, v);
  _max = std::max(_max, v);
}
  
void reducer::add( value_type v, size_t count) 
{
  _empty = false;
  this->add_(v);
  _total_count += count;
  if ( count > 1)
    _lossy_count += count - 1;
}
  
void reducer::add( const data_type& values, size_t count)
{
  _empty = false;
  for (value_type v : values)
    this->add_(v);
  _total_count += count;
  if ( values.size() < count )
    _lossy_count += count - values.size();
  
}
  
void reducer::add( const data_type& values)
{
  this->add( values, values.size() );
}
  
void reducer::add( std::initializer_list<value_type> values )
{
  for (value_type v : values)
    this->add_(v);
  _empty = false;
  _total_count += values.size();
}
  
void reducer::add( const reduced_data& v )
{
  this->add( v.data, v.count);
  _lossy_count += v.lossy;
  if ( v.min != 0 )
    this->minmax( v.min );
  if ( v.max != 0 )
    this->minmax( v.max );
}
  
reducer::reduced_ptr reducer::detach()
{
  if ( /*this->empty()*/ _empty )
    return nullptr;
    
  auto res = reduced_ptr(new reduced_type);
  this->reduce();
  if ( !_data.empty() )
    _data.front()->swap(res->data);
  res->avg = static_cast<value_type>(_average);
  res->count = _total_count;
  res->lossy = _lossy_count;
  res->max = _max;
  res->min = _min;
  this->clear();
  return std::move(res);
}
  
bool reducer::empty() const
{
  return _empty;
}
  
void reducer::reduce()
{
  if ( _data.empty() )
    return;

  std::sort( _data.back()->begin(), _data.back()->end() );
    
  if ( _data.size() == 1 )
    return;
    
  // Расчетное становится реальным
  _lossy_count = this->lossy_count();
    
  for ( size_t i = 1, l = 1; i < _opt.reducer_limit; ++i )
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
  
void reducer::add_( value_type v) 
{
  _average += (v - _average) / (_average_count + 1);
  ++_average_count;
    
  this->minmax(v);
    
  // Если заполнен текущий уровень 
  if ( _data.empty() || _data.back()->size() == _opt.reducer_limit  )
  {
    // Создаем новый уровень 
    if ( _data.size() == _opt.reducer_levels || _opt.reducer_limit == 0)
    {
      // Лимит уровней закончился
      ++_lossy_count;
      return;
    }
      
    if ( !_data.empty() )
      std::sort( _data.back()->begin(), _data.back()->end() );
    _data.push_back( _allocator.create() );
  }
    
  _data.back()->push_back(v);
}
  

}

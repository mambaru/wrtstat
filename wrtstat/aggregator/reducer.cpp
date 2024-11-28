#include <wrtstat/aggregator/reducer.hpp>

namespace wrtstat {

namespace{
static const size_t MagicModeNumber = 16UL;
}

reducer::reducer(const reducer_options& opt, const allocator& a  )
  : _opt( opt )
  , _allocator( a )
{
  // Если число уровней меньше 1/16, то в nth режим никогда не переключится
  if ( opt.reducer_mode == reducer_options::mode::adapt )
  {
    if ( _opt.reducer_levels == 0 || _opt.reducer_limit/_opt.reducer_levels  > MagicModeNumber )
      const_cast<reducer_options&>(_opt).reducer_mode = reducer_options::mode::sorting;
    else if ( opt.initial_mode == reducer_options::mode::adapt )
      _current_mode = reducer_options::mode::nth;
    else
      _current_mode = reducer_options::mode::sorting;
  }
  else if ( opt.reducer_mode == reducer_options::mode::nth )
    _current_mode = reducer_options::mode::nth;
  // по умолчанию _current_mode=sorting

}

std::unique_ptr<reducer> reducer::clone()
{
  std::unique_ptr<reducer> cln = std::unique_ptr<reducer>( new reducer(_opt, _allocator) );
  cln->_empty = _empty;
  cln->_min = _min;
  cln->_max = _max;
  cln->_lossy_count = _lossy_count;
  cln->_total_count = _total_count;
  cln->_average_count = _average_count;
  cln->_average = _average;
  for (const auto& d : this->_data )
    cln->_data.push_back( data_ptr( new data_type(*d) ) );
  return cln;
}

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

size_t reducer::capacity() const
{
  size_t result = 0;
  for ( auto& p : _data )
  {
    if ( p!=nullptr )
      result += p->capacity();
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
  // Не используем _lossy_count += v.lossy;
  // а считаем как разницу v.count и v.data.size()
  this->add( v.data, v.count);
  if ( v.min != 0 )
    this->minmax( v.min );
  if ( v.max != 0 )
    this->minmax( v.max );
}

reducer::reduced_ptr reducer::get_reduced()
{
  auto tmp = this->clone();
  return tmp->detach();
}

reducer::reduced_ptr reducer::detach()
{
  if ( _empty )
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
  return res;
}

bool reducer::empty() const
{
  return _empty;
}

 /*
  * x  x
  *  x  x
  *   x  x
  * 012345
  */
void reducer::nth_element_(size_t l, size_t i)
{
  if ( i < l )
  {
    std::nth_element(_data[l]->begin(), _data[l]->begin()+std::ptrdiff_t(i),  _data[l]->end());
  }
  else
  {
    std::nth_element(_data[l]->begin() + std::ptrdiff_t(i - l), _data[l]->begin() + std::ptrdiff_t(i),  _data[l]->end());
  }
}

value_type reducer::get_nth_(size_t l, size_t i)
{
  if ( _current_mode == reducer_options::mode::nth )
    this->nth_element_(l, i);
  return _data[l]->at(i);
}

void reducer::reduce()
{

  if ( _data.empty() )
    return;

  if ( _current_mode == reducer_options::mode::sorting )
    std::sort( _data.back()->begin(), _data.back()->end() );

  if ( _data.size() == 1 )
    return;

  // Расчетное становится реальным
  _lossy_count = this->lossy_count();

  size_t current_levels = _data.size();
  for ( size_t i = 0, l = current_levels; i < _opt.reducer_limit; ++i )
  {
    if ( l == current_levels ) // первую строку не трогаем
    {
      if ( _current_mode == reducer_options::mode::nth )
        this->nth_element_(0, i);
      l=1;
    }
    else if (l == current_levels - 1 )
    {
      if ( i < _data[l]->size() )
        _data[0]->at(i)=get_nth_(l++, i);
    }
    else
    {
      _data[0]->at(i)=get_nth_(l++, i);
    }
  }

  if (_opt.reducer_mode == reducer_options::mode::adapt)
  {
    _current_mode = _data.size()==0 || _opt.reducer_limit / _data.size() > MagicModeNumber
      ? reducer_options::mode::sorting
      : reducer_options::mode::nth;
  }

  for ( size_t i = 1; i < _data.size(); ++i)
    _allocator.free( std::move(_data[i]) );

  _data.resize(1);
}

void reducer::add_( value_type v)
{
  _average += ( static_cast<double>(v) - _average) / static_cast<double>( _average_count + 1 );
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

    if ( _current_mode == reducer_options::mode::sorting && !_data.empty() )
      std::sort( _data.back()->begin(), _data.back()->end() );
    _data.push_back( _allocator.create() );
    _data.back()->reserve(_opt.reducer_limit);
  }

  _data.back()->push_back(v);
}


}

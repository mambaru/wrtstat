#pragma once
#include <vector>
#include <limits>
#include <memory>
#include <algorithm>

namespace wrtstat {

template<typename T = std::vector<long long int> >
class collector
{
  struct realy{};
  struct fake{};
public:
  typedef T data_type;
  typedef typename data_type::value_type value_type;

public:

  collector(size_t limit, size_t reserve=0)
    : _limit( limit )
    , _reserve( reserve )
  {
  }

  size_t lossy_count() const { return _lossy_count; }
  size_t total_count() const { return _total_count; }
  size_t level() const { return _level; }
  size_t max() const { return _max; }
  size_t min() const { return _min; }
  size_t position() const { return _position; }
  size_t size() const { return _data.size(); }

  const data_type& values() const { return _data;}

  void detach(data_type& d)
  {
    _data.swap(d);
    this->clear();
  }

  void attach(data_type& d)
  {
    this->clear();
    _data.swap(d);
    for ( value_type v : d)
      this->add_<fake>(d);
  }

  void clear()
  {
    _min = std::numeric_limits<value_type>::max();
    _max = std::numeric_limits<value_type>::max();
    _lossy_count = 0;
    _total_count = 0;
    _level = 0;
    _position = 0;
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
    this->add_<realy>(v);
  }

private:
  
  template<typename F>
  void push_back_(value_type v) { this->push_back_(v, F() );}
  void push_back_(value_type, fake) {}
  void push_back_(value_type v, realy) 
  {
    if ( _data.capacity() < _reserve )
      _data.reserve(_reserve);
    _data.push_back(v);
  }

  template<typename F>
  void set_in_cur_pos_(value_type v) { this->set_in_cur_pos_(v, F() );}
  void set_in_cur_pos_(value_type, fake) {}
  void set_in_cur_pos_(value_type v, realy) { _data[_position]=v;}

  template<typename F>
  void filtered_add_( value_type v) 
  {
    if ( 0 == ( (_position + 1) % (_level+1) ) )
      this->set_in_cur_pos_<F>(v);
    else if ( _position < _level )
    {
      if ( _level > 1 )
      {
        auto l = _level - 1;
        //this->set_in_cur_pos_<F>(v);
        auto a1 = (_data.size() / l ) * l;
        auto a2 = _data.size() - a1;
        if ( _position == a2 && _position!=0)
        {
          std::cout << "set " << v << " to " << _position << " for level " << (_level - 1) << std::endl;
          this->set_in_cur_pos_<F>(v);
        }
        else
          ++_lossy_count;
      }
      else
        ++_lossy_count;
    }
    else
      ++_lossy_count;

    ++_position;
    if ( _position == _limit )
    {
      ++_level;
      _position = 0;
    }
  }

  template<typename F>
  void add_( value_type v) 
  {
    ++_total_count;
    this->minmax(v);
    if ( _data.size() < _limit )
    {
      this->push_back_<F>(v);
    }
    else
    {
      if ( _level == 0 )
      {
        ++_level;
        _position = 0;
      }
      this->filtered_add_<F>(v);
    }
  }

private:
  value_type _min = std::numeric_limits<value_type>::max();
  value_type _max = std::numeric_limits<value_type>::max();
  // Счетчик отброшенных после заполнения
  size_t _lossy_count = 0;
  size_t _total_count = 0;
  // Текущий уровень фильтрации
  size_t _level = 0;
  // Ограничение на размер данных
  size_t _limit = 0;
  // 
  size_t _reserve = 0;
  //
  size_t _position = 0;
  // Значения
  data_type _data;
};


template<typename C = collector<>, typename T = time_t >
class seperator
{
public:
  typedef T time_type;
  typedef C collector_type;
  typedef typename collector_type::data_type  data_type;
  typedef typename collector_type::value_type value_type;
  
  seperator(time_type now, time_type step, size_t limit, size_t reserve=0)
    : _collector(limit, reserve)
    , _step_ts(step)
    , _next_time(now + step)
  {
  }

  void set_time(time_type now) 
  {
    _next_time = now + _step_ts;
  }
  
  const collector_type& collector() const { return _collector;}

  bool add(time_type now, value_type v)
  {
    bool ready = this->aggregate(now, false);
    _collector.add(v);
    return ready;
  }

  bool aggregate(time_type now, bool force)
  {
    if ( !force && now < _next_time )
      return false;
    this->aggregate_();
    return true;
  }
private:
  
  collector_type _collector;
  time_type _step_ts;
  time_type _next_time;

};

/*
 void aggregate_()
  {
    aggregated_data<data_type> ag;
    ag.count = _collector.total_count();
    ag.losses = _collector.lossy_count();
    _collector.detach(ag.data);
    value_type& d = ag.data;
    this->init_perc_(ag, ag.data);
  }
  
  void init_perc_(aggregated& ag, data_type& d)
  {
    size_t off = 0;
    ag.perc50 = this->perc_next_(50, d, off);
    ag.perc80 = this->perc_next_(80, d, off);
    ag.perc95 = this->perc_next_(95, d, off);
    ag.perc99 = this->perc_next_(99, d, off);
  }
  
  value_type perc_next_(size_t perc, data_type& d, size_t& off)
  {
    size_t pos = (d.size()*perc)/100;
    std::nth_element(d.begin() + off, d.begin() + pos, d.end());
    off = pos;
    return d[pos];
  }
  

 */
struct aggregated
{
  size_t avg = 0;
  size_t count = 0;
  size_t perc50 = 0;
  size_t perc80 = 0;
  size_t perc95 = 0;
  size_t perc99 = 0;
  size_t perc100 = 0;
  size_t min = 0;
  size_t losses = 0;

  bool empty() const
  {
    return avg == 0 
           && count == 0
           && perc50 == 0 
           && perc80 == 0 
           && perc95 == 0 
           && perc99 == 0 
           && perc100 == 0 
           && min == 0
           && losses == 0;
  }
};

template<typename T>
struct aggregated_data: aggregated
{
  typedef T data_type;
  data_type data;
};

}

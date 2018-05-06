#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/reducer_options.hpp>
#include <wrtstat/manager/pool.hpp>
#include <vector>
#include <limits>
#include <memory>
#include <algorithm>
#include <iostream>


namespace wrtstat {

class reducer
{
public:
  typedef std::vector<data_ptr> data_list;
  typedef aggregated_data reduced_type;
  typedef std::unique_ptr<reduced_type> reduced_ptr;

public:

  reducer(const reducer_options& opt, const allocator& a = allocator() );

  size_t lossy_count() const;

  size_t total_count() const;

  size_t levels() const;

  value_type max() const;

  value_type min() const;
  
  size_t size() const;

  bool filled() const;
  
  void clear();

  void minmax(value_type v);

  void add( value_type v, size_t count);

  void add( const data_type& values, size_t count);

  void add( const data_type& values);

  void add( std::initializer_list<value_type> values );
  
  void add( const reduced_data& v );

  reduced_ptr detach();
  
  reduced_ptr get_reduced();

  bool empty() const;

  void reduce();

  std::unique_ptr<reducer> clone();  
private:
  

  value_type nth_element_(size_t l, size_t i);

  void add_( value_type v);

private:
  const reducer_options _opt;
  bool _empty = true;
  value_type _min = std::numeric_limits<value_type>::max();
  value_type _max = std::numeric_limits<value_type>::min();
  // Счетчик отброшенных после заполнения
  size_t _lossy_count = 0;
  size_t _total_count = 0;
  allocator _allocator;
  size_t _average_count = 0;
  double _average = 0.0;
  data_list _data;
};


}

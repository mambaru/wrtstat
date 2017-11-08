#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/aggregator_options.hpp>
#include <wrtstat/separator.hpp>
#include <iostream>
#include <algorithm>

namespace wrtstat {

class aggregator_base
{
public:
  typedef separator::reduced_type reduced_type;
  typedef separator::reduced_ptr reduced_ptr;
  typedef std::function< void(time_type now, time_type value, size_type count) > value_adder_t;
  typedef std::function< void(time_type now, data_type&& value, size_type count) > data_adder_t;
  typedef std::function< void(const reduced_data&) > reduced_adder_t;
  typedef aggregator_options options_type;
  typedef aggregated_data::ptr aggregated_ptr;
  typedef aggregated_data::handler aggregated_handler;
  typedef std::list< aggregated_ptr > aggregated_list;
  
  aggregator_base(time_type ts_now, const aggregator_options& opt, const allocator& a = allocator() );

  const separator& get_separator() const;

  bool add(time_type ts_now, value_type v, size_type count);
  bool add(time_type ts_now, const data_type& v, size_type count);
  bool add( const reduced_data& v);
  bool add( const reduced_data& v, aggregated_handler handler);

  bool separate(time_type ts_now, bool force);
  
  aggregated_ptr pop();

  aggregated_ptr force_pop();

  void enable(bool value);

  size_t size() const;

  void clear( time_type ts_now );

  template<typename D>
  static time_type now() 
  {
    return separator::now<D>();
  }

  static time_type now(time_type resolution) 
  {
    return separator::now(resolution);
  }

protected:
  template<typename T>
  value_adder_t create_value_adder( std::weak_ptr<T> wthis );

  template<typename T>
  data_adder_t create_data_adder( std::weak_ptr<T> wthis );
  
  template<typename T>
  reduced_adder_t create_reduced_adder( std::weak_ptr<T> wthis );

private:

  static value_type nth_(size_type perc, size_type& off, data_type& d);

  // TODO: return сколько элементов было удалено + добавить в count
  void reduce_(data_type& d) const;

  void aggregate0_();

  void aggregate1_(aggregated_handler handler);

  aggregated_ptr aggregate2_(reduced_ptr d) const;

public:
  separator _sep;
  size_t _reduced_size = 0;
  bool _enabled = true;
  aggregated_list _ag_list;
  std::shared_ptr<int> _id;
};

class aggregator
  : public aggregator_base
  , public std::enable_shared_from_this<aggregator>
{
public:
  typedef aggregator_base::options_type options_type;
  
  aggregator(time_type ts_now, const options_type& opt, const allocator& a = allocator());
  
  value_adder_t create_value_adder( );

  data_adder_t create_data_adder( );
  
  reduced_adder_t create_reduced_adder( );
};

class aggregator_mt
  : private aggregator_base
  , public std::enable_shared_from_this<aggregator_mt>
{
public:
  typedef std::mutex mutex_type;
  
  typedef aggregator_base::reduced_type reduced_type;
  typedef aggregator_base::reduced_ptr reduced_ptr;
  typedef aggregator_base::options_type options_type;
  typedef aggregator_base::aggregated_ptr aggregated_ptr;
  typedef aggregator_base::aggregated_handler aggregated_handler;
  typedef aggregator_base::value_adder_t value_adder_t;
  typedef aggregator_base::data_adder_t data_adder_t;
  typedef aggregator_base::reduced_adder_t reduced_adder_t;


  aggregator_mt(time_type ts_now, const options_type& opt, const allocator& a = allocator() );

  bool add(time_type ts_now, value_type v, size_type count);

  bool add(time_type ts_now, const data_type& v, size_type count);
  
  bool add( const reduced_data& v, aggregated_handler handler);
  
  bool add( const reduced_data& v);

  aggregated_ptr pop();

  aggregated_ptr force_pop();

  bool separate(time_type ts_now, bool force);

  void enable(bool value);

  value_adder_t create_value_adder( );

  data_adder_t create_data_adder( );
  
  reduced_adder_t create_reduced_adder( );

  template<typename D>
  static time_type now() 
  {
    return aggregator_base::now<D>();
  }

  size_t size() const;

  void clear(time_type ts_now);

private:
  mutable mutex_type _mutex;
};

}

#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/manager/manager.hpp>
#include <wrtstat/manager/dict.hpp>
#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <wrtstat/meters/composite_meter.hpp>
#include <wrtstat/meters/multi_meter.hpp>
#include <wrtstat/aggregator.hpp>
#include <mutex>
#include <chrono>
#include <deque>
#include <memory>

namespace wrtstat {

template<typename Manager>
class wrtstat_base
{
public:
  typedef Manager manager_type;
  
  typedef typename manager_type::time_type time_type;
  typedef typename manager_type::value_type value_type;
  typedef typename manager_type::size_type size_type;
  typedef typename manager_type::aggregated_type aggregated_type;
  typedef typename manager_type::aggregated_ptr aggregated_ptr;
  
  typedef wrtstat_options options_type;

  typedef std::shared_ptr<manager_type> manager_ptr;
  typedef std::mutex mutex_type;

  wrtstat_base(options_type opt = options_type() ) 
    : _prefixes(opt.prefixes)
  { 
    _m = std::make_shared<manager_type>(opt);
    if ( _prefixes.empty() ) 
      _prefixes.push_back("");
  }

  bool add(int id, time_type ts_now, value_type v, size_type count)
  {
    return _m->add(id, ts_now, v, count);
  }

  aggregated_ptr pop(int id)
  {
    return _m->pop(id);
  }

  aggregated_ptr force_pop(int id)
  {
    return _m->force_pop(id);
  }

  size_t count() const 
  {
    return _m->size();
  }

  int create_aggregator(std::string name, time_type ts_now)
  {
    return this->create_aggregator_( std::move(name), ts_now);
  }

  std::string get_name(int id) const 
  {
    return _m->get_name(id);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > 
    create_time_meter(int id, time_type ts_now, size_type count)
  {
    return this->create_time_meter_<D>(id, ts_now, count);
  }

  std::shared_ptr< size_meter > 
    create_size_meter(int id, time_type ts_now, size_type size)
  {
    return this->create_size_meter_(id, ts_now, size);
  }

  std::shared_ptr< size_meter > 
    create_value_meter(int id, time_type ts_now, size_type value, size_type count)
  {
    return this->create_size_meter_(id, ts_now, value, count);
  }
  
  template<typename D >
  std::shared_ptr< composite_meter<D> > 
    create_composite_meter(int time_id, int read_id, int write_id, time_type ts_now, size_type size)
  {
    return this->create_composite_meter_<D>(time_id, read_id, write_id, ts_now, size);
  }

  template<typename D >
  std::shared_ptr< composite_meter<D> >
    create_composite_meter( const std::string& time_name, 
                            const std::string& read_name,
                            const std::string& write_name,
                            time_type ts_now, size_type size)
  {
    return this->create_composite_meter_<D>( 
      this->create_aggregator_( time_name, ts_now), 
      this->create_aggregator_( read_name, ts_now), 
      this->create_aggregator_( write_name, ts_now), 
      ts_now, count, size);
  }

  template<typename D>
  std::shared_ptr< multi_meter< composite_meter<D> > > 
    create_multi_meter( const std::string& time_name, 
                        const std::string& read_name,
                        const std::string& write_name,
                        time_type ts_now,
                        size_type size
                      )
  {
    return this->create_multi_meter_<D>( time_name, read_name, write_name, ts_now, size);
  }
  
  template< typename MeterType, typename... A>
  std::shared_ptr< multi_meter<MeterType> >
    create_multi_meter(const std::string& meter_name, 
                        time_type ts_now,
                        A... args)
  {
    return this->create_multi_meter_<MeterType>( meter_name, ts_now, args...);
  }

  void enable(bool value)
  {
    _m->enable(value);
  };
  
  template<typename D>
  static time_type now() 
  {
    return aggregator::now<D>();
  }

  
private:

  int create_aggregator_(const std::string& name, time_type ts_now)
  {
    if ( name.empty() )
      return -1;
    return _m->create_aggregator( name, ts_now);
  }

  template<typename D >
  std::shared_ptr< time_meter<D> >
    create_time_meter_(int id, time_type ts_now, size_type count)
  {
    return std::make_shared< time_meter<D> >(_m->create_meter(id), ts_now, count );
  }


  std::shared_ptr< value_meter >
    create_value_meter_(int id, time_type ts_now, size_type value, size_type count)
  {
    return std::make_shared< value_meter >(_m->create_meter(id), ts_now, value, count );
  }
  
  std::shared_ptr< size_meter >
    create_size_meter_(int id, time_type ts_now, size_type size)
  {
    return std::make_shared< size_meter >(_m->create_meter(id), ts_now, size);
  }

  template<typename D >
  std::shared_ptr< composite_meter<D> >
    create_composite_meter_(int time_id, int read_id, int write_id, time_type ts_now, size_type size)
  {
    return std::make_shared<composite_meter<D> >( 
      time_id!=-1 ? this->create_time_meter_< D >(time_id, ts_now, 1) : nullptr,
      read_id!=-1 ? this->create_size_meter_(read_id, ts_now, size) : nullptr,
      write_id!=-1 ? this->create_size_meter_(write_id, ts_now, 0 ) : nullptr
    );
  }

  
  template<typename D >
  std::shared_ptr< multi_meter< composite_meter<D> > >
    create_multi_meter_(const std::string& time_name, 
                        const std::string& read_name,
                        const std::string& write_name,
                        time_type ts_now, 
                        size_type size)
  {
    auto meter = std::make_shared< multi_meter< composite_meter<D> > >();
    for ( auto prefix : _prefixes )
    {
      int time_id = -1;
      int read_id = -1;
      int write_id = -1;
      if ( !time_name.empty() )
        time_id = this->create_aggregator_(prefix + time_name, ts_now );
      if ( !read_name.empty() )
        read_id = this->create_aggregator_(prefix + read_name, ts_now );
      if ( !write_name.empty() )
        write_id = this->create_aggregator_(prefix + write_name, ts_now );

      meter->push_back( this->create_composite_meter_<D>( time_id, read_id, write_id, ts_now, size) );
    }
    return meter;
  }

    
  template< typename MeterType, typename... A>
  std::shared_ptr< multi_meter<MeterType> >
    create_multi_meter_(const std::string& meter_name, 
                        time_type ts_now, 
                        A... args)
  {
    auto meter = std::make_shared< multi_meter<MeterType> >();
    for ( auto prefix : _prefixes )
    {
      int meter_id = this->create_aggregator_(prefix + meter_name, ts_now );
      auto m = std::make_shared< MeterType >(_m->create_meter(meter_id), ts_now, args... );
      meter->push_back(m);
    }
    return meter;

  }

public:
  manager_ptr _m;
  std::vector<std::string> _prefixes;
};

class wrtstat_st: public wrtstat_base<manager_st>
{
public:
  typedef wrtstat_base::options_type options_type;
  wrtstat_st(options_type opt = options_type() ) 
    : wrtstat_base<manager_st>(opt)
  {}

};

class wrtstat_mt: public wrtstat_base<manager_mt>
{
public:
  typedef wrtstat_base::options_type options_type;
  wrtstat_mt(options_type opt = options_type() ) 
    : wrtstat_base<manager_mt>(opt)
  {}
  
};


}

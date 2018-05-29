#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/manager/aggregator_registry.hpp>
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

class meter_manager
  : public aggregator_registry
{
public:
  typedef aggregator_registry super;
  typedef aggregator_registry manager_type;
  typedef typename manager_type::aggregated_ptr aggregated_ptr;

  //typedef typename manager_type::aggregated_handler aggregated_handler;
  typedef std::function<void(const std::string& name, aggregated_data::ptr)> named_aggregated_handler;
  typedef wrtstat_options options_type;

  typedef std::shared_ptr<manager_type> manager_ptr;
  typedef std::mutex mutex_type;

  explicit meter_manager(const options_type& opt = options_type() ) 
    :  aggregator_registry(opt, opt.pool_size, opt.id_init, opt.id_step)
    , _resolution(opt.resolution)
    , _handler(opt.handler)
    , _prefixes(opt.prefixes)
  { 
    if ( _prefixes.empty() ) 
      _prefixes.push_back("");
  }
  
// ----------------------------------
// time_meter
// ----------------------------------

  template<typename D >
  time_meter<D> create_time_meter(id_t id, time_type ts_now, size_type cnt)
  {
    return time_meter<D>(super::create_simple_pusher(id, make_handler_(id) ), ts_now, cnt);
  }

  template<typename D >
  time_meter_factory<D>
  create_time_meter_factory(id_t id)
  {
    return time_meter_factory<D>(super::create_simple_pusher(id, make_handler_(id) ), _resolution );
  }

  template<typename D >
  time_meter<D>
    create_time_meter(const std::string& name, time_type ts_now, size_type cnt)
  {
    return this->create_time_meter<D>( this->create_aggregator(name, ts_now), ts_now, cnt);
  }

  template<typename D >
  time_meter_factory<D>
    create_time_meter_factory(const std::string& name)
  {
    return this->create_time_meter_factory<D>( this->create_aggregator(name, aggregator::now(_resolution) ) );
  }
  
  template<typename D>
  multi_meter_factory< time_meter_factory<D> > 
    create_time_multi_meter_factory( const std::string& time_name)
  {
    multi_meter_factory< time_meter_factory<D> > meter;
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      auto f = this->create_time_meter_factory<D>(prefix + time_name);
      meter.push_back( std::move(f) );
    }
    return meter;
  }
  
  template<typename D>
  multi_meter< time_meter<D> > 
    create_time_multi_meter( const std::string& time_name, time_type ts_now, size_type count)
  {
    auto meter = multi_meter< time_meter<D> >();
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      id_t meter_id = super::create_aggregator(prefix + time_name, ts_now );
      meter.push_back(
        time_meter<D>( super::create_simple_pusher(meter_id, make_handler_(meter_id)), ts_now, count )
      );
    }
    return meter;

  }


// ----------------------------------
// size_meter
// ----------------------------------
  size_meter
    create_size_meter(id_t id, time_type ts_now, size_type size)
  {
    return size_meter(super::create_simple_pusher(id, make_handler_(id) ), ts_now, size);
  }

  size_meter_factory
    create_size_meter_factory(id_t id)
  {
    return size_meter_factory(super::create_simple_pusher(id, make_handler_(id) ), _resolution);
  }
  
  size_meter create_size_meter(const std::string& name, time_type ts_now, size_type size)
  {
    return this->create_size_meter( this->create_aggregator(name, ts_now), ts_now, size);
  }
  
  multi_meter<size_meter>
    create_size_multi_meter(const std::string& meter_name, 
                        time_type ts_now, size_type size)
  {
    auto meter = multi_meter<size_meter>();
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      id_t meter_id = super::create_aggregator(prefix + meter_name, ts_now );
      meter.push_back(
        size_meter( super::create_simple_pusher(meter_id, make_handler_(meter_id)), ts_now, size )
      );
    }
    return meter;
  }
  
  size_meter_factory create_size_meter_factory(const std::string& name)
  {
    return this->create_size_meter_factory( this->create_aggregator(name, aggregator::now(_resolution)));
  }
  
  multi_meter_factory< size_meter_factory > 
    create_size_multi_meter_factory( const std::string& time_name)
  {
    multi_meter_factory< size_meter_factory > factory;
    factory.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      auto f = this->create_size_meter_factory(prefix + time_name);
      factory.push_back( std::move(f) );
    }
    return factory;
  }

// ----------------------------------
// value_meter
// ----------------------------------

  value_meter create_value_meter(id_t id, time_type ts_now, value_type value, size_type cnt)
  {
    return value_meter(super::create_simple_pusher(id, make_handler_(id)), ts_now, value, cnt );
  }

  value_meter_factory create_value_meter_factory(id_t id)
  {
    return value_meter_factory(super::create_simple_pusher(id, make_handler_(id)), _resolution );
  }
  
  value_meter create_value_meter(const std::string& name, time_type ts_now, value_type value, size_type count)
  {
    return this->create_value_meter( this->create_aggregator(name, ts_now), ts_now, value, count);
  }
  
  multi_meter<value_meter>
    create_value_multi_meter(const std::string& meter_name, 
                        time_type ts_now, value_type value, size_type count)
  {
    auto meter = multi_meter<value_meter>();
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      id_t meter_id = super::create_aggregator(prefix + meter_name, ts_now );
      meter.push_back(
        value_meter( super::create_simple_pusher(meter_id, make_handler_(meter_id)), ts_now, value, count )
      );
    }
    return meter;
  }

  value_meter_factory create_value_meter_factory(const std::string& name)
  {
    return this->create_value_meter_factory( this->create_aggregator(name, aggregator::now(_resolution)));
  }
  
  multi_meter_factory< value_meter_factory > 
    create_value_multi_meter_factory( const std::string& time_name)
  {
    multi_meter_factory< value_meter_factory > factory;
    factory.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      auto f = this->create_value_meter_factory(prefix + time_name);
      factory.push_back( std::move(f) );
    }
    return factory;
  }

// ----------------------------------
// composite_meter
// ----------------------------------
  template<typename D >
  composite_meter<D> create_composite_meter(id_t time_id, id_t read_id, id_t write_id, time_type ts_now, 
                                            size_type count, size_type readed, size_type writed, bool summary_size)
  {
    return composite_meter<D>(
        super::create_composite_pusher(
          time_id, read_id, write_id, 
          make_handler_(time_id), 
          make_handler_(read_id),
          make_handler_(write_id), 
          summary_size
        ), 
        ts_now, count, readed,  writed
    );
  }

  template<typename D >
  composite_meter<D>
    create_composite_meter( const std::string& time_name, 
                            const std::string& read_name,
                            const std::string& write_name,
                            time_type ts_now,
                            size_type count, size_type readed, size_type writed, bool summary_size
                          )
  {
    return composite_meter<D>(
        super::create_composite_pusher(
          time_name, read_name, write_name, 
          make_handler_(time_name), 
          make_handler_(read_name), 
          make_handler_(write_name), 
          summary_size, ts_now
        ), 
        ts_now, count, readed,  writed
    );
  }

  template<typename D >
  composite_meter_factory<D> 
    create_composite_meter_factory(id_t time_id, id_t read_id, id_t write_id, bool summary_size)
  {
    return composite_meter_factory<D>(
        super::create_composite_pusher(
          time_id, read_id, write_id, 
          make_handler_(time_id), make_handler_(read_id), make_handler_(write_id), 
          summary_size
        ), 
        _resolution 
    );
  }

  template<typename D >
  composite_meter_factory<D>
    create_composite_meter_factory( 
                            const std::string& time_name, 
                            const std::string& read_name,
                            const std::string& write_name,
                            bool summary_size
                          )
  {
    return composite_meter_factory<D>(
        super::create_composite_pusher(
          time_name, read_name, write_name, 
          make_handler_(time_name), 
          make_handler_(read_name),
          make_handler_(write_name), 
          summary_size, 
          aggregator::now(_resolution)
        ), 
        _resolution 
      );
  }

  template<typename D>
  multi_meter< composite_meter<D> > 
    create_composite_multi_meter( const std::string& time_name, 
                        const std::string& read_name,
                        const std::string& write_name,
                        time_type ts_now,
                        size_type count, 
                        size_type readed, 
                        size_type writed, 
                        bool summary_size
                      )
  {
    auto meter = multi_meter< composite_meter<D> >();
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      id_t time_id = static_cast<id_t>(-1);
      id_t read_id = static_cast<id_t>(-1);
      id_t write_id = static_cast<id_t>(-1);
      if ( !time_name.empty() )
        time_id = super::create_aggregator(prefix + time_name, ts_now );
      if ( !read_name.empty() )
        read_id = super::create_aggregator(prefix + read_name, ts_now );
      if ( !write_name.empty() )
        write_id = super::create_aggregator(prefix + write_name, ts_now );

      meter.push_back( this->create_composite_meter<D>( time_id, read_id, write_id, ts_now, count, readed, writed, summary_size) );
    }
    return meter;
  }
  
  /*
  template<typename D>
  multi_meter< time_meter<D> > 
    create_multi_meter( const std::string& time_name, time_type ts_now, size_type count)
  {
    return this->create_multi_meter< time_meter<D> >(time_name, ts_now, count);
  }
  */

    /*
  template< typename MeterType, typename... A>
  multi_meter<MeterType>
    create_multi_meter(const std::string& meter_name, 
                        time_type ts_now,
                        A... args)
  {
    auto meter = multi_meter<MeterType>();
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      id_t meter_id = super::create_aggregator(prefix + meter_name, ts_now );
      meter.push_back(
        MeterType( super::create_simple_pusher(meter_id, make_handler_(meter_id)), ts_now, args... )
      );
    }
    return meter;
  }*/
  
  static std::string make_name_(const std::string& prefix, const std::string& name)
  {
    if (name.empty()) 
      return std::string();
    return prefix + name;
  }
  
  template<typename D>
  multi_meter_factory< composite_meter_factory<D> >
    create_composite_multi_meter_factory( 
      const std::string& time_name, 
      const std::string& read_name,
      const std::string& write_name,
      bool summary_size
    )
  {
    multi_meter_factory< composite_meter_factory<D> > meter;
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      auto f = this->create_composite_meter_factory<D>(
        make_name_(prefix, time_name), 
        make_name_(prefix, read_name), 
        make_name_(prefix, write_name), 
        summary_size);
      meter.push_back( std::move(f) );
    }
    return meter;
  }

  template<typename D>
  static time_type now_t() 
  {
    return aggregator::now_t<D>();
  }
  
private:
  
  aggregated_data::handler make_handler_( id_t id ) const
  {
    if (id == bad_id || _handler==nullptr)
      return nullptr;
    std::string name;
    return [name, id, this] ( aggregated_data::ptr ag) mutable
    {
      if (id != bad_id)
      {
        if ( name.empty() ) 
        {
          name = this->get_name(id);
          if ( name.empty() ) 
          {
            id = bad_id;
            return;
          }
        }
        this->_handler( name, std::move(ag) );
      }
    };
  }

  aggregated_data::handler make_handler_( const std::string& name) const
  {
    if ( auto h = _handler )
    {
      return [name, h]( aggregated_data::ptr ag)
      {
        h( name, std::move(ag) );
      };
    }
    return nullptr;
  }
  
private:
  resolutions _resolution = resolutions::none;
  named_aggregated_handler _handler;
  std::vector<std::string> _prefixes;
};

}

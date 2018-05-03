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

  typedef typename manager_type::aggregated_handler aggregated_handler;
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
  
// time_meter
  template<typename D >
  std::shared_ptr< time_meter<D> > 
    create_time_meter(id_t id, time_type ts_now, size_type cnt)
  {
    if (_handler!=nullptr)
      return std::make_shared< time_meter<D> >(super::create_simple_pusher(id, _handler), ts_now, cnt );
    else
      return std::make_shared< time_meter<D> >(super::create_simple_adder(id), ts_now, cnt );
  }

  template<typename D >
  std::shared_ptr< time_meter_factory<D> > 
    create_time_meter_factory(id_t id)
  {
    if (_handler!=nullptr)
      return std::make_shared< time_meter_factory<D> >(super::create_simple_pusher(id, _handler), _resolution );
    else
      return std::make_shared< time_meter_factory<D> >(super::create_simple_adder(id), _resolution );
  }

  template<typename D >
  std::shared_ptr< time_meter<D> > 
    create_time_meter(const std::string& name, time_type ts_now, size_type cnt)
  {
    return this->create_time_meter<D>( this->create_aggregator(name, ts_now), ts_now, cnt);
  }

  template<typename D >
  std::shared_ptr< time_meter_factory<D> > 
    create_time_meter_factory(const std::string& name)
  {
    return this->create_time_meter_factory<D>( this->create_aggregator(name, aggregator::now(_resolution) ) );
  }

// size_meter
  std::shared_ptr< size_meter > 
    create_size_meter(id_t id, time_type ts_now, size_type size)
  {
    if (_handler!=nullptr)
      return std::make_shared< size_meter >(super::create_simple_pusher(id, _handler), ts_now, size);
    else
      return std::make_shared< size_meter >(super::create_simple_adder(id), ts_now, size);
  }

  std::shared_ptr< size_meter_factory > 
    create_size_meter_factory(id_t id)
  {
    if (_handler!=nullptr)
      return std::make_shared< size_meter_factory >(super::create_simple_pusher(id, _handler), _resolution);
    else
      return std::make_shared< size_meter_factory >(super::create_simple_adder(id), _resolution);
  }
  
  std::shared_ptr< size_meter > 
    create_size_meter(const std::string& name, time_type ts_now, size_type size)
  {
    return this->create_size_meter( this->create_aggregator(name, ts_now), ts_now, size);
  }

  std::shared_ptr< size_meter_factory > 
    create_size_meter_factory(const std::string& name)
  {
    return this->create_size_meter_factory( this->create_aggregator(name, aggregator::now(_resolution)));
  }

// value_meter
  std::shared_ptr< value_meter > 
    create_value_meter(id_t id, time_type ts_now, size_type value, size_type cnt)
  {
    if (_handler!=nullptr)
      return std::make_shared< value_meter >(super::create_simple_pusher(id, _handler), ts_now, value, cnt );
    else
      return std::make_shared< value_meter >(super::create_simple_adder(id), ts_now, value, cnt );
  }

  std::shared_ptr< value_meter_factory > 
    create_value_meter_factory(id_t id)
  {
    if (_handler!=nullptr)
      return std::make_shared< value_meter_factory >(super::create_simple_pusher(id, _handler), _resolution );
    else
      return std::make_shared< value_meter_factory >(super::create_simple_adder(id), _resolution );
  }
  
  std::shared_ptr< value_meter > 
    create_value_meter(const std::string& name, time_type ts_now, size_type value, size_type count)
  {
    return this->create_value_meter( this->create_aggregator(name, ts_now), ts_now, value, count);
  }

  std::shared_ptr< value_meter_factory > 
    create_value_meter_factory(const std::string& name)
  {
    return this->create_value_meter_factory( this->create_aggregator(name, aggregator::now(_resolution)));
  }

// composite_meter
  template<typename D >
  std::shared_ptr< composite_meter<D> > 
    create_composite_meter(id_t time_id, id_t read_id, id_t write_id, time_type ts_now, 
                           size_type count, size_type readed, size_type writed)
  {
    if (_handler!=nullptr)
      return std::make_shared< composite_meter<D> >(
        super::create_composite_pusher(time_id, read_id, write_id, _handler), 
        ts_now, count, readed,  writed);
    else
      return std::make_shared< composite_meter<D> >(
        super::create_composite_adder(time_id, read_id, write_id), 
        ts_now, count, readed,  writed);

    /*
    return std::make_shared<composite_meter<D> >( 
      time_id  != bad_id  ? this->create_time_meter< D >(time_id, ts_now, 1) : nullptr,
      read_id  != bad_id  ? this->create_size_meter(read_id, ts_now, size) : nullptr,
      write_id != bad_id  ? this->create_size_meter(write_id, ts_now, 0 ) : nullptr
    );*/
  }

  template<typename D >
  std::shared_ptr< composite_meter<D> >
    create_composite_meter( const std::string& time_name, 
                            const std::string& read_name,
                            const std::string& write_name,
                            time_type ts_now,
                            size_type count, size_type readed, size_type writed
                          )
  {
    if (_handler!=nullptr)
      return std::make_shared< composite_meter<D> >(
        super::create_composite_pusher(time_name, read_name, write_name, ts_now, _handler), 
        ts_now, count, readed,  writed);
    else
      return std::make_shared< composite_meter<D> >(
        super::create_composite_adder(time_name, read_name, write_name, ts_now), 
        ts_now, count, readed,  writed);

    /*
    return this->create_composite_meter<D>( 
      super::create_aggregator( time_name, ts_now), 
      super::create_aggregator( read_name, ts_now), 
      super::create_aggregator( write_name, ts_now), 
      ts_now, s);
    */
  }

    template<typename D >
  std::shared_ptr< composite_meter_factory<D> > 
    create_composite_meter_factory(id_t time_id, id_t read_id, id_t write_id)
  {
    if (_handler!=nullptr)
      return std::make_shared< composite_meter_factory<D> >(
        super::create_composite_pusher(time_id, read_id, write_id, _handler), _resolution );
    else
      return std::make_shared< composite_meter_factory<D> >(
        super::create_composite_adder(time_id, read_id, write_id), _resolution );

    /*
    return std::make_shared<composite_meter_factory<D> >( 
      time_id  != bad_id  ? this->create_time_meter_factory< D >(time_id) : nullptr,
      read_id  != bad_id  ? this->create_size_meter_factory(read_id) : nullptr,
      write_id != bad_id  ? this->create_size_meter_factory(write_id) : nullptr,
      _resolution
    );
    */
  }

  template<typename D >
  std::shared_ptr< composite_meter_factory<D> >
    create_composite_meter_factory( 
                            const std::string& time_name, 
                            const std::string& read_name,
                            const std::string& write_name
                          )
  {
    if (_handler!=nullptr)
      return std::make_shared< composite_meter_factory<D> >(
        super::create_composite_pusher(time_name, read_name, write_name, aggregator::now(_resolution), _handler), _resolution );
    else
      return std::make_shared< composite_meter_factory<D> >(
        super::create_composite_adder(time_name, read_name, write_name, aggregator::now(_resolution)), _resolution );

    /*
    time_type ts_now = aggregator::now(_resolution);
    return this->create_composite_meter_factory<D>( 
      super::create_aggregator( time_name, ts_now), 
      super::create_aggregator( read_name, ts_now), 
      super::create_aggregator( write_name, ts_now)
    );
    */
  }

// multi_meter

  template<typename D>
  std::shared_ptr< multi_meter< composite_meter<D> > > 
    create_multi_meter( const std::string& time_name, 
                        const std::string& read_name,
                        const std::string& write_name,
                        time_type ts_now,
                        size_type size
                      )
  {
    auto meter = std::make_shared< multi_meter< composite_meter<D> > >();
    meter->reserve( _prefixes.size() );
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

      meter->push_back( this->create_composite_meter<D>( time_id, read_id, write_id, ts_now, size) );
    }
    return meter;

  }
  
  template< typename MeterType, typename... A>
  std::shared_ptr< multi_meter<MeterType> >
    create_multi_meter(const std::string& meter_name, 
                        time_type ts_now,
                        A... args)
  {
    auto meter = std::make_shared< multi_meter<MeterType> >();
    meter->reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      id_t meter_id = super::create_aggregator(prefix + meter_name, ts_now );
      auto m = std::make_shared< MeterType >( _handler!=nullptr ? super::create_simple_pusher(meter_id, _handler) : super::create_simple_adder(meter_id), ts_now, args... );
      meter->push_back(m);
    }
    return meter;
  }
  
  template<typename D>
  static time_type now_t() 
  {
    return aggregator::now_t<D>();
  }

public:
  time_type _resolution = 0;
  aggregated_handler _handler; // WARNING: нигде не инициализирован!
  std::vector<std::string> _prefixes;
};

}

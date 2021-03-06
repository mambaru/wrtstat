#include<wrtstat/meter_manager.hpp>
#include<chrono>

namespace wrtstat {

////////////////////////////////////////////////////////

// Канонический вариант:
//  template multi_meter< composite_meter<std::chrono::nanoseconds> > 
//    meter_manager::create_composite_multi_meter<std::chrono::nanoseconds>(
//      const std::string&, const std::string&, const std::string&, bool);
// Сокращенный:
//   this->create_composite_multi_meter< std::chrono::nanoseconds>(std::string(),std::string(),std::string(),false);
//
  
void meter_manager::fake_implementations_()
{
  this->create_time_meter<std::chrono::seconds>(0);
  this->create_time_meter<std::chrono::milliseconds>(0);
  this->create_time_meter<std::chrono::microseconds>(0);
  this->create_time_meter< std::chrono::nanoseconds>(0);

  this->create_time_meter<std::chrono::seconds>(std::string());
  this->create_time_meter<std::chrono::milliseconds>(std::string());
  this->create_time_meter<std::chrono::microseconds>(std::string());
  this->create_time_meter< std::chrono::nanoseconds>(std::string());

  this->create_time_multi_meter<std::chrono::seconds>(std::string());
  this->create_time_multi_meter<std::chrono::milliseconds>(std::string());
  this->create_time_multi_meter<std::chrono::microseconds>(std::string());
  this->create_time_multi_meter< std::chrono::nanoseconds>(std::string());
  
  this->create_composite_meter<std::chrono::seconds>(0,0,0,false);
  this->create_composite_meter<std::chrono::milliseconds>(0,0,0,false);
  this->create_composite_meter<std::chrono::microseconds>(0,0,0,false);
  this->create_composite_meter< std::chrono::nanoseconds>(0,0,0,false);

  this->create_composite_meter<std::chrono::seconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_meter<std::chrono::milliseconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_meter<std::chrono::microseconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_meter< std::chrono::nanoseconds>(std::string(),std::string(),std::string(),false);

  this->create_composite_multi_meter<std::chrono::seconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_multi_meter<std::chrono::milliseconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_multi_meter<std::chrono::microseconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_multi_meter< std::chrono::nanoseconds>(std::string(),std::string(),std::string(),false);

  meter_manager::now_t<std::chrono::seconds>() ;
  meter_manager::now_t<std::chrono::milliseconds>() ;
  meter_manager::now_t<std::chrono::microseconds>() ;
  meter_manager::now_t<std::chrono::nanoseconds>() ;
}

  
/////////////////////////////////////////////////
  
meter_manager::meter_manager(const options_type& opt ) 
  :  aggregator_registry(opt, opt.pool_size, opt.id_init, opt.id_step)
  , _resolution(opt.resolution)
  , _handler(opt.handler)
  , _prefixes(opt.prefixes)
{ 
  if ( _prefixes.empty() ) 
    _prefixes.push_back("");
}

template<typename D >
time_meter<D>
meter_manager::create_time_meter(id_t id)
{
  return time_meter<D>(super::create_simple_pusher(id, make_handler_(id) ), _resolution );
}

template<typename D >
time_meter<D>
meter_manager::create_time_meter(const std::string& name)
{
  return this->create_time_meter<D>( this->create_aggregator(name, aggregator::now(_resolution) ) );
}
  
template<typename D>
multi_meter< time_meter<D> > 
meter_manager::create_time_multi_meter( const std::string& time_name)
{
  multi_meter< time_meter<D> > meter;
  meter.reserve( _prefixes.size() );
  for ( auto prefix : _prefixes )
  {
    auto f = this->create_time_meter<D>(prefix + time_name);
    meter.push_back( std::move(f) );
  }
  return meter;
}
  
size_meter meter_manager::create_size_meter(id_t id)
{
  return size_meter(super::create_simple_pusher(id, make_handler_(id) ), _resolution);
}
  
size_meter meter_manager::create_size_meter(const std::string& name)
{
  return this->create_size_meter( this->create_aggregator(name, aggregator::now(_resolution)));
}
  
multi_meter< size_meter > 
  meter_manager::create_size_multi_meter( const std::string& time_name)
{
  multi_meter< size_meter > factory;
  factory.reserve( _prefixes.size() );
  for ( auto prefix : _prefixes )
  {
    auto f = this->create_size_meter(prefix + time_name);
    factory.push_back( std::move(f) );
  }
  return factory;
}

// ----------------------------------
// value_meter
// ----------------------------------

value_meter meter_manager::create_value_meter(id_t id)
{
  return value_meter(super::create_simple_pusher(id, make_handler_(id)), _resolution );
}
  
value_meter meter_manager::create_value_meter(const std::string& name)
{
  return this->create_value_meter( this->create_aggregator(name, aggregator::now(_resolution)));
}
  
multi_meter< value_meter > 
  meter_manager::create_value_multi_meter( const std::string& time_name)
{
  multi_meter< value_meter > factory;
  factory.reserve( _prefixes.size() );
  for ( auto prefix : _prefixes )
  {
    auto f = this->create_value_meter(prefix + time_name);
    factory.push_back( std::move(f) );
  }
  return factory;
}

// ----------------------------------
// composite_meter
// ----------------------------------
  
template<typename D >
composite_meter<D> 
  meter_manager::create_composite_meter(id_t time_id, id_t read_id, id_t write_id, bool summary_size)
{
  return composite_meter<D>(
          super::create_composite_pusher(
            time_id, read_id, write_id, 
            make_handler_(time_id), make_handler_(read_id), make_handler_(write_id), 
            summary_size
          ), 
          _resolution 
        );
}
  
template<typename D >
composite_meter<D>
  meter_manager::create_composite_meter( 
                            const std::string& time_name, 
                            const std::string& read_name,
                            const std::string& write_name,
                            bool summary_size
                          )
  {
    return composite_meter<D>(
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
  meter_manager::create_composite_multi_meter( 
      const std::string& time_name, 
      const std::string& read_name,
      const std::string& write_name,
      bool summary_size
    )
{
    multi_meter< composite_meter<D> > meter;
    meter.reserve( _prefixes.size() );
    for ( auto prefix : _prefixes )
    {
      auto f = this->create_composite_meter<D>(
        make_name_(prefix, time_name), 
        make_name_(prefix, read_name), 
        make_name_(prefix, write_name), 
        summary_size);
      meter.push_back( std::move(f) );
    }
    return meter;
}
  
template<typename D>
time_type meter_manager::now_t() 
{
  return aggregator::now_t<D>();
}
  
time_type meter_manager::now(time_type resolution) 
{
  return aggregator::now(resolution);
}

time_type meter_manager::now(resolutions resolution)
{
  return aggregator::now(resolution);
}
  
time_type meter_manager::now()
{
  return aggregator::now(_resolution);
}

std::string meter_manager::make_name_(const std::string& prefix, const std::string& name)
{
    if (name.empty()) 
      return std::string();
    return prefix + name;
}
  
aggregated_data::handler meter_manager::make_handler_( id_t id ) const
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

aggregated_data::handler meter_manager::make_handler_( const std::string& name) const
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
 
}

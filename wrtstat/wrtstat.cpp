#include <wrtstat/wrtstat.hpp>
#include <fas/system/memory.hpp>
#include <chrono>

namespace wrtstat {

////////////////////////////////////////////////////////

// Канонический вариант:
//  template multi_meter< composite_meter<std::chrono::nanoseconds> >
//    wrtstat::create_composite_multi_meter<std::chrono::nanoseconds>(
//      const std::string&, const std::string&, const std::string&, bool);
// Сокращенный:
//   this->create_composite_multi_meter< std::chrono::nanoseconds>(std::string(),std::string(),std::string(),false);
//

void wrtstat::fake_implementations_()
{
  this->create_time_meter<std::chrono::seconds>(0);
  this->create_time_meter<std::chrono::milliseconds>(0);
  this->create_time_meter<std::chrono::microseconds>(0);
  this->create_time_meter< std::chrono::nanoseconds>(0);

  this->create_time_meter<std::chrono::seconds>(std::string());
  this->create_time_meter<std::chrono::milliseconds>(std::string());
  this->create_time_meter<std::chrono::microseconds>(std::string());
  this->create_time_meter< std::chrono::nanoseconds>(std::string());

  this->create_time_multi_meter<std::chrono::seconds>({""}, std::string());
  this->create_time_multi_meter<std::chrono::milliseconds>({""},std::string());
  this->create_time_multi_meter<std::chrono::microseconds>({""},std::string());
  this->create_time_multi_meter< std::chrono::nanoseconds>({""},std::string());

  this->create_composite_meter<std::chrono::seconds>(0,0,0,false);
  this->create_composite_meter<std::chrono::milliseconds>(0,0,0,false);
  this->create_composite_meter<std::chrono::microseconds>(0,0,0,false);
  this->create_composite_meter< std::chrono::nanoseconds>(0,0,0,false);

  this->create_composite_meter<std::chrono::seconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_meter<std::chrono::milliseconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_meter<std::chrono::microseconds>(std::string(),std::string(),std::string(),false);
  this->create_composite_meter< std::chrono::nanoseconds>(std::string(),std::string(),std::string(),false);

  this->create_composite_multi_meter<std::chrono::seconds>({""},std::string(),std::string(),std::string(),false);
  this->create_composite_multi_meter<std::chrono::milliseconds>({""},std::string(),std::string(),std::string(),false);
  this->create_composite_multi_meter<std::chrono::microseconds>({""},std::string(),std::string(),std::string(),false);
  this->create_composite_multi_meter< std::chrono::nanoseconds>({""},std::string(),std::string(),std::string(),false);

  wrtstat::now_t<std::chrono::seconds>() ;
  wrtstat::now_t<std::chrono::milliseconds>() ;
  wrtstat::now_t<std::chrono::microseconds>() ;
  wrtstat::now_t<std::chrono::nanoseconds>() ;
}


/////////////////////////////////////////////////

wrtstat::~wrtstat()
{
  this->force_pushout();
}

wrtstat::wrtstat(const options_type& opt )
  :  _registry( std::make_unique<registry_type>(opt/*, opt.pool_size, opt.id_init, opt.id_step*/) )
  , _resolution(opt.resolution)
  , _handler(opt.handler)
  /*, _prefixes(opt.prefixes)*/
{
  /*if ( _prefixes.empty() )
    _prefixes.push_back("");*/
}

void wrtstat::set_initializer(initializer_fun_t&& init_f)
{
  return _registry->set_initializer( std::move(init_f) );  
}

id_t wrtstat::create_aggregator(const std::string& name, time_type now)
{
  return _registry->create_aggregator(name, now);
}

std::string wrtstat::get_name(id_t id) const
{
  return _registry->get_name(id);
}


template<typename D >
time_meter<D>
wrtstat::create_time_meter(id_t id)
{
  return time_meter<D>(_registry->create_simple_pusher(id, make_handler_(id) ), _resolution );
}

template<typename D >
time_meter<D>
wrtstat::create_time_meter(const std::string& name)
{
  return this->create_time_meter<D>( _registry->create_aggregator(name, aggregator::now(_resolution) ) );
}

template<typename D>
multi_meter< time_meter<D> >
wrtstat::create_time_multi_meter( const prefix_list_t& prefixes, const std::string& time_name)
{
  multi_meter< time_meter<D> > meter;
  meter.reserve( prefixes.size() );
  for ( auto prefix : prefixes )
  {
    auto f = this->create_time_meter<D>(prefix + time_name);
    meter.push_back( std::move(f) );
  }
  return meter;
}

size_meter wrtstat::create_size_meter(id_t id)
{
  return size_meter(_registry->create_simple_pusher(id, make_handler_(id) ), _resolution);
}

size_meter wrtstat::create_size_meter(const std::string& name)
{
  return this->create_size_meter( _registry->create_aggregator(name, aggregator::now(_resolution)));
}

multi_meter< size_meter >
  wrtstat::create_size_multi_meter( const prefix_list_t& prefixes, const std::string& time_name)
{
  multi_meter< size_meter > factory;
  factory.reserve( prefixes.size() );
  for ( auto prefix : prefixes )
  {
    auto f = this->create_size_meter(prefix + time_name);
    factory.push_back( std::move(f) );
  }
  return factory;
}

// ----------------------------------
// value_meter
// ----------------------------------

value_meter wrtstat::create_value_meter(id_t id)
{
  return value_meter(_registry->create_simple_pusher(id, make_handler_(id)), _resolution );
}

value_meter wrtstat::create_value_meter(const std::string& name)
{
  return this->create_value_meter( _registry->create_aggregator(name, aggregator::now(_resolution)));
}

multi_meter< value_meter >
  wrtstat::create_value_multi_meter( const prefix_list_t& prefixes, const std::string& time_name)
{
  multi_meter< value_meter > factory;
  factory.reserve( prefixes.size() );
  for ( auto prefix : prefixes )
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
  wrtstat::create_composite_meter(id_t time_id, id_t read_id, id_t write_id, bool summary_size)
{
  return composite_meter<D>(
          _registry->create_composite_pusher(
            time_id, read_id, write_id,
            make_handler_(time_id), make_handler_(read_id), make_handler_(write_id),
            summary_size
          ),
          _resolution
        );
}

template<typename D >
composite_meter<D>
  wrtstat::create_composite_meter(
                            const std::string& time_name,
                            const std::string& read_name,
                            const std::string& write_name,
                            bool summary_size
                          )
  {
    return composite_meter<D>(
        _registry->create_composite_pusher(
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
  wrtstat::create_composite_multi_meter(
      const prefix_list_t& prefixes,
      const std::string& time_name,
      const std::string& read_name,
      const std::string& write_name,
      bool summary_size
    )
{
    multi_meter< composite_meter<D> > meter;
    meter.reserve( prefixes.size() );
    for ( auto prefix : prefixes )
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
time_type wrtstat::now_t()
{
  return aggregator::now_t<D>();
}

time_type wrtstat::now(time_type resolution) const
{
  return aggregator::now(resolution);
}

time_type wrtstat::now(resolutions resolution) const
{
  return aggregator::now(resolution);
}

time_type wrtstat::now() const
{
  return aggregator::now(_resolution);
}

size_t wrtstat::pushout() const
{
  return this->pushout_(false);
}

size_t wrtstat::force_pushout() const
{
  return this->pushout_(true);
}

const wrtstat::registry_type& wrtstat::registry() const
{
  return *_registry;
}

size_t wrtstat::aggregators_count() const
{
  return _registry->aggregators_count();
}

bool wrtstat::add(id_t id, time_type now, value_type v, size_type count)
{
  return _registry->add(id, now, v, count);
}

bool wrtstat::add( const std::string& name, const reduced_data& v)
{
  return _registry->add(name, v);
}

wrtstat::aggregated_ptr wrtstat::pop(id_t id) const
{
  return _registry->pop(id);
}

wrtstat::aggregated_ptr wrtstat::force_pop(id_t id) const
{
  return _registry->force_pop(id);
}

void wrtstat::pop_all(named_aggregated_list* ag_list) const
{
  return _registry->pop_all(ag_list);
}

void wrtstat::force_pop_all(named_aggregated_list* ag_list) const
{
  return _registry->force_pop_all(ag_list);
}

void wrtstat::enable(bool value)
{
  _registry->enable(value);
}

bool wrtstat::del(const std::string& name)
{
  return _registry->del(name);
}


std::string wrtstat::make_name_(const std::string& prefix, const std::string& name)
{
    if (name.empty())
      return std::string();
    return prefix + name;
}

aggregated_data::handler wrtstat::make_handler_( id_t id ) const
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
          name = _registry->get_name(id);
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

aggregated_data::handler wrtstat::make_handler_( const std::string& name) const
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

size_t wrtstat::pushout_(bool force) const
{
  aggregator_registry::named_aggregated_list ag_list;
  force ? _registry->force_pop_all(&ag_list) : _registry->pop_all(&ag_list);
  for(auto &nag : ag_list)
  {
    _handler(nag.first, std::move(nag.second) );
  }
  return ag_list.size();
}

}



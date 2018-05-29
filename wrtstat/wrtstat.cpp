
#include <wrtstat/wrtstat.hpp>
#include <wrtstat/meter_manager.hpp>
#include <ctime>
#include <cstdlib>

namespace wrtstat{

/*
class wrtstat::impl
  : public meter_manager
{
public:
  impl(const wrtstat_options& opt )
    : meter_manager( opt)
  {}
};

wrtstat::wrtstat(options_type opt)
{
  _impl = std::make_shared<impl>(opt);
  _resolution = opt.resolution;
}

bool wrtstat::add( const std::string& name, const reduced_data& v)
{
  return _impl->add(name, v);
}

bool wrtstat::del( const std::string& name )
{
  return _impl->del(name);
}

void wrtstat::enable(bool val)
{
  _impl->enable(val);
}

composite_multi_meter_ptr 
  wrtstat::create_composite_prototype(const std::string& time_name, const std::string& read_name, const std::string& write_name)
{
  if (time_name.empty() && read_name.empty() && write_name.empty() ) 
    return nullptr;
  auto now = aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter<duration_type>(time_name, read_name, write_name, now, 0);
  meter->reset();
  return meter;
}

time_multi_meter_ptr 
  wrtstat::create_time_prototype(const std::string& time_name)
{
  if (time_name.empty() ) 
    return nullptr;

  auto now = aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter< time_meter<duration_type> >(time_name, now, 0);
  meter->reset();
  return meter;
}

size_multi_meter_ptr 
  wrtstat::create_size_prototype(const std::string& size_name)
{
  if (size_name.empty() ) 
    return nullptr;

  auto now = aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter< size_meter >(size_name, now, 0);
  meter->reset();
  return meter;

}

value_multi_meter_ptr 
  wrtstat::create_value_prototype(const std::string& value_name)
{
  if (value_name.empty() ) 
    return nullptr;

  auto now = aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter< value_meter >(value_name, now, 0);
  meter->reset();
  return meter;
}


composite_multi_meter_ptr 
  wrtstat::create_meter(composite_multi_meter_ptr m, size_type size )
{
  if ( m == nullptr ) return nullptr;
  auto now = aggregator::now(_resolution);
  return m->clone(now, size);
}

time_multi_meter_ptr 
  wrtstat::create_meter(time_multi_meter_ptr m, size_type count )
{
  if ( m == nullptr ) return nullptr;
  auto now = aggregator::now(_resolution);
  return m->clone(now, count);
}

size_multi_meter_ptr 
  wrtstat::create_meter(size_multi_meter_ptr m, size_type size )
{
  if ( m == nullptr ) return nullptr;
  auto now = aggregator::now(_resolution);
  return m->clone(now, size);
}

value_multi_meter_ptr 
  wrtstat::create_meter(value_multi_meter_ptr m, size_type value, size_type count )
{
  if ( m == nullptr ) return nullptr;
  auto now = aggregator::now(_resolution);
  return m->clone(now, value, count);
}

wrtstat::value_adder_t wrtstat::create_value_adder(const std::string& name, time_type ts_now)
{
  return _impl->create_value_adder( name, ts_now );
}

wrtstat::data_adder_t wrtstat::create_data_adder(const std::string& name, time_type ts_now)
{
  return _impl->create_data_adder( name, ts_now );
}

wrtstat::reduced_adder_t wrtstat::create_reduced_adder( const std::string& name, time_type ts_now)
{
  return _impl->create_reduced_adder( name, ts_now );
}

size_t wrtstat::aggregators_count() const
{
  return _impl->aggregators_count();
}

wrtstat::aggregated_ptr wrtstat::pop(id_t id)
{
  return _impl->pop(id);
}

std::string wrtstat::get_name(id_t id) const
{
  return _impl->get_name(id);
}
*/


}

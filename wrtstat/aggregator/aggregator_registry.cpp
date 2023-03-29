#include "aggregator_registry.hpp"
#include <wrtstat/system/rwlock.hpp>

namespace wrtstat {

aggregator_registry::aggregator_registry(const options_type& opt)
  : _opt(opt)
  , _dict(opt.id_init, opt.id_step)
  , _pool(opt.reducer_limit, opt.pool_size)
  , initializer_(opt.initializer)
{ }

size_t aggregator_registry::aggregators_count() const
{
  read_lock<mutex_type> lk(_mutex);
  return _agarr.size();
}

bool aggregator_registry::add(id_t id, time_type now, value_type v, size_type count)
{
  if ( auto p = this->get_aggregator(id) )
    return p->add(now, v, count);
  return false;
}

bool aggregator_registry::add( const std::string& name, const reduced_data& v)
{
  if ( auto p = this->get_aggregator( name, v.ts ) )
    return p->add(v);
  return false;
}

aggregator_registry::aggregated_ptr aggregator_registry::force_pop(id_t id) const
{
  if ( auto p = this->get_aggregator(id) )
    return p->force_pop();
  return nullptr;
}

aggregator_registry::aggregated_ptr aggregator_registry::pop(id_t id) const
{
  if ( auto p = this->get_aggregator(id) )
    return p->pop();
  return nullptr;
}

void aggregator_registry::pop_all(named_aggregated_list* ag_list) const
{
  if ( ag_list == nullptr ) return;

  read_lock<mutex_type> lk(_mutex);
  this->pop_(ag_list, &aggregator_type::pop, false);

}

void aggregator_registry::force_pop_all(named_aggregated_list* ag_list) const
{
  if ( ag_list == nullptr ) return;

  read_lock<mutex_type> lk(_mutex);
  this->pop_(ag_list, &aggregator_type::force_pop, true);
}


std::string aggregator_registry::get_name(id_t id) const
{
  read_lock<mutex_type> lk(_mutex);
  return _dict.get_name(id);
}

void aggregator_registry::set_initializer(initializer_fun_t&& init_f)
{
  std::lock_guard<mutex_type> lk(_mutex);
  initializer_ = init_f;
}
 
id_t aggregator_registry::create_aggregator(const std::string& name, time_type now)
{
  if ( name.empty() )
    return bad_id;

  std::lock_guard<mutex_type> lk(_mutex);
  id_t id = _dict.create_id( name );
  size_t pos = _dict.id2pos(id);
  if ( pos < _agarr.size() && _agarr[pos]!=nullptr )
    return id;

  if ( _agarr.size() <= pos )
    _agarr.resize( pos + 1 );

  if ( _agarr[pos] == nullptr )
  {
    _agarr[pos] = std::make_shared<aggregator_type>(now, _opt, _pool.get_allocator() );
    if ( !_enabled )
      _agarr[pos]->enable(false);
    else if ( initializer_ != nullptr )
    {
      reduced_data rd;
      if ( initializer_(name, &rd) )
      {
        _agarr[pos]->add(rd);
      }
    }
    
  }
  return id;
}

aggregator_registry::aggregator_ptr aggregator_registry::get_aggregator(const std::string& name, time_type now)
{
  return this->get_aggregator( this->create_aggregator(name, now) );
}

aggregator_registry::aggregator_ptr aggregator_registry::get_aggregator(id_t id) const
{
  if ( id == bad_id )
    return nullptr;

  read_lock<mutex_type> lk(_mutex);
  size_t pos = _dict.id2pos(id);
  if ( pos >= _agarr.size() )
  {
    return nullptr;
  }
  return _agarr[ pos ];
}

aggregator_registry::composite_adder_t
  aggregator_registry::make_composite_handler_(
    simple_pusher_t time_adder,
    simple_pusher_t read_adder,
    simple_pusher_t write_adder,
    bool summary_size
  )
{
  return [time_adder, read_adder, write_adder, summary_size]
    (time_type now, time_type span, size_type count,
      size_type readed, size_type writed)
    {
      if ( time_adder!=nullptr )
        time_adder(now, span, count);
      if ( read_adder!=nullptr )
        read_adder(now, static_cast<time_type>(readed), summary_size ? readed : count);
      if ( write_adder!=nullptr )
        write_adder(now, static_cast<time_type>(writed), summary_size ? writed : count);
    };
}


aggregator_registry::simple_pusher_t aggregator_registry::create_simple_pusher( id_t id, aggregated_handler handler )
{
  if ( auto ag = this->get_aggregator(id) )
    return ag->create_simple_pusher(handler);
  return nullptr;
}

aggregator_registry::data_pusher_t aggregator_registry::create_data_pusher( id_t id, aggregated_handler handler )
{
  if ( auto ag = this->get_aggregator(id) )
    return ag->create_data_pusher(handler);
  return nullptr;
}

aggregator_registry::reduced_pusher_t aggregator_registry::create_reduced_pusher( id_t id, aggregated_handler handler )
{
  if ( auto ag = this->get_aggregator(id) )
    return ag->create_reduced_pusher(handler);
  return nullptr;
}

aggregator_registry::composite_pusher_t aggregator_registry::create_composite_pusher(
  id_t time_id, id_t read_id, id_t write_id,
  aggregated_handler time_handler,
  aggregated_handler read_handler,
  aggregated_handler write_handler,
  bool summary_size
)
{
  simple_pusher_t time_pusher;
  simple_pusher_t read_pusher;
  simple_pusher_t write_pusher;

  if ( auto ag = this->get_aggregator(time_id) )
    time_pusher = ag->create_simple_pusher(time_handler);

  if ( auto ag = this->get_aggregator(read_id) )
    read_pusher = ag->create_simple_pusher(read_handler);

  if ( auto ag = this->get_aggregator(write_id) )
    write_pusher = ag->create_simple_pusher(write_handler);

  if (time_pusher==nullptr && read_pusher==nullptr && write_pusher==nullptr)
    return nullptr;

  return this->make_composite_handler_(time_pusher, read_pusher, write_pusher, summary_size);
}

//
aggregator_registry::simple_pusher_t aggregator_registry::create_simple_pusher(
  const std::string& name,
  aggregated_handler handler,
  time_type ts_now)
{
  return this->create_simple_pusher( this->create_aggregator(name, ts_now), handler );
}

aggregator_registry::data_pusher_t aggregator_registry::create_data_pusher(
  const std::string& name,
  aggregated_handler handler,
  time_type ts_now)
{
  return this->create_data_pusher( this->create_aggregator(name, ts_now), handler);
}

aggregator_registry::reduced_pusher_t aggregator_registry::create_reduced_pusher(
  const std::string& name,
  aggregated_handler handler,
  time_type ts_now)
{
  return this->create_reduced_pusher(this->create_aggregator(name, ts_now), handler);
}

aggregator_registry::composite_pusher_t aggregator_registry::create_composite_pusher(
  const std::string& time_name,
  const std::string& read_name,
  const std::string& write_name,
  aggregated_handler time_handler,
  aggregated_handler read_handler,
  aggregated_handler write_handler,
  bool summary_size,
  time_type ts_now
)
{
  return this->create_composite_pusher(
    this->create_aggregator(time_name, ts_now),
    this->create_aggregator(read_name, ts_now),
    this->create_aggregator(write_name, ts_now),
    time_handler, read_handler, write_handler, summary_size
  );
}

void aggregator_registry::enable(bool value)
{
  std::lock_guard<mutex_type> lk(_mutex);

  this->_enabled = value;
  for (auto a : _agarr)
  {
    if ( a!=nullptr )
      a->enable(value);
  }
}

bool aggregator_registry::del(const std::string& name)
{
  std::lock_guard<mutex_type> lk(_mutex);

  id_t id = this->_dict.get_id(name);
  if ( id == static_cast<id_t>(-1) )
    return false;
  size_t pos = _dict.id2pos(id);
  _agarr[ pos ] = nullptr;
  return this->_dict.free(id);
}

void aggregator_registry::pop_(named_aggregated_list* ag_list, aggregated_ptr (aggregator_type::*pop_fun)(), bool force) const
{
  size_t size = _agarr.size();
  ag_list->reserve(size);
  for ( size_t i = 0 ; i < size; ++i)
  {
    size_t id = _dict.pos2id(i);
    if ( auto p = this->get_aggregator(id) )
    {
      p->separate(force);
      if (aggregated_ptr ag = (p.get()->*pop_fun)())
      {
        ag_list->push_back( std::make_pair(_dict.get_name(id), std::move(ag) ) );
      }
    }
  }
}

}

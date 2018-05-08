
#include <wrtstat/manager/aggregator_registry.hpp>
#include <wrtstat/types.hpp>

namespace wrtstat {


  aggregator_registry::aggregator_registry(const options_type& opt, size_t pool_size, id_t init, id_t step)
    : _opt(opt)
    , _dict(init, step)
    , _pool(opt.reducer_limit, pool_size)
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

  aggregator_registry::aggregated_ptr aggregator_registry::force_pop(id_t id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->force_pop();
    return nullptr;
  }

  aggregator_registry::aggregated_ptr aggregator_registry::pop(id_t id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->pop();
    return nullptr;
  }

  std::string aggregator_registry::get_name(id_t id) const 
  {
    read_lock<mutex_type> lk(_mutex);
    return _dict.get_name(id);
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

  /*
  aggregator_registry::simple_adder_t aggregator_registry::create_simple_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_simple_adder();
    return nullptr;
  }

  aggregator_registry::data_adder_t aggregator_registry::create_data_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_data_adder();
    return nullptr;
  }
  
  aggregator_registry::reduced_adder_t aggregator_registry::create_reduced_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_reduced_adder();
    return nullptr;
  }
  
  aggregator_registry::composite_adder_t aggregator_registry::create_composite_adder( id_t time_id, id_t read_id, id_t write_id, bool summary_size )
  {
    simple_adder_t time_adder;
    simple_adder_t read_adder;
    simple_adder_t write_adder;
    
    if ( auto ag = this->get_aggregator(time_id) )
      time_adder = ag->create_simple_adder();

    if ( auto ag = this->get_aggregator(read_id) )
      read_adder = ag->create_simple_adder();

    if ( auto ag = this->get_aggregator(write_id) )
      write_adder = ag->create_simple_adder();

    if (time_adder==nullptr && read_adder==nullptr && write_adder==nullptr)
      return nullptr;
    
    return this->make_composite_handler_(time_adder, read_adder, write_adder, summary_size);
  }

  aggregator_registry::simple_adder_t aggregator_registry::create_value_adder(const std::string& name, time_type ts_now)
  {
    return this->create_simple_adder( this->create_aggregator(name, ts_now) );
  }

  aggregator_registry::data_adder_t aggregator_registry::create_data_adder(const std::string& name, time_type ts_now)
  {
    return this->create_data_adder( this->create_aggregator(name, ts_now));
  }
  
  aggregator_registry::reduced_adder_t aggregator_registry::create_reduced_adder( const std::string& name, time_type ts_now )
  {
    return this->create_reduced_adder(this->create_aggregator(name, ts_now));
  }
  
  aggregator_registry::composite_adder_t aggregator_registry::create_composite_adder( 
    const std::string& time_name, 
    const std::string& read_name, 
    const std::string& write_name, 
    bool summary_size,
    time_type ts_now )
  {
    return this->create_composite_adder(
      this->create_aggregator(time_name, ts_now),
      this->create_aggregator(read_name, ts_now),
      this->create_aggregator(write_name, ts_now),
      summary_size
    );
  }
  */
  
  aggregator_registry::composite_adder_t
    aggregator_registry::make_composite_handler_(simple_adder_t time_adder, simple_adder_t read_adder, simple_adder_t write_adder, bool summary_size  )
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
  aggregator_registry::simple_pusher_t aggregator_registry::create_simple_pusher(const std::string& name, aggregated_handler handler, time_type ts_now)
  {
    return this->create_simple_pusher( this->create_aggregator(name, ts_now), handler );
  }

  aggregator_registry::data_pusher_t aggregator_registry::create_data_pusher(const std::string& name, aggregated_handler handler, time_type ts_now)
  {
    return this->create_data_pusher( this->create_aggregator(name, ts_now), handler);
  }
  
  aggregator_registry::reduced_pusher_t aggregator_registry::create_reduced_pusher( const std::string& name, aggregated_handler handler, time_type ts_now )
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
 

}

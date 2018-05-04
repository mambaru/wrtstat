#pragma once

#include <wrtstat/manager/dict.hpp>
#include <wrtstat/manager/pool.hpp>
#include <mutex>
#include <wrtstat/manager/mutex/empty_mutex.hpp>
#include <wrtstat/manager/mutex/rwlock.hpp>
#include <wrtstat/manager/mutex/spinlock.hpp>
#include <wrtstat/aggregator.hpp>
#include <wrtstat/types.hpp>
#include <deque>
#include <array>

namespace wrtstat {

class aggregator_registry
{
public:
  typedef rwlock<std::mutex> mutex_type;
  typedef aggregator_mt aggregator_type;
  typedef typename aggregated_data::ptr aggregated_ptr;
  typedef aggregator_options options_type;
  
  typedef typename aggregator_type::simple_adder_t simple_adder_t;
  typedef typename aggregator_type::data_adder_t data_adder_t;
  typedef typename aggregator_type::reduced_adder_t reduced_adder_t;
  typedef typename aggregator_type::simple_pusher_t simple_pusher_t;
  typedef typename aggregator_type::data_pusher_t data_pusher_t;
  typedef typename aggregator_type::reduced_pusher_t reduced_pusher_t;
  typedef std::function< void(time_type now, time_type value, size_type count, 
                              size_type readed, size_type writed) > composite_adder_t;
  typedef composite_adder_t composite_pusher_t;
  typedef typename aggregator_type::aggregated_handler aggregated_handler;
  //typedef std::function<void(const std::string& name, aggregated_data::ptr)> aggregated_handler;
  
  typedef std::shared_ptr<aggregator_type> aggregator_ptr;
  typedef std::deque<aggregator_ptr> aggregator_list;

  explicit aggregator_registry(const options_type& opt, size_t pool_size, id_t init, id_t step)
    : _opt(opt)
    , _dict(init, step)
    , _pool(opt.reducer_limit, pool_size)
  { }

  size_t aggregators_count() const 
  {
    read_lock<mutex_type> lk(_mutex);
    return _agarr.size();
  }

  bool add(id_t id, time_type now, value_type v, size_type count)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->add(now, v, count);
    return false;
  }
  
  bool add( const std::string& name, const reduced_data& v)
  {
    if ( auto p = this->create_get_aggregator( name, v.ts ) )
      return p->add(v);
    return false;  
  }

  aggregated_ptr force_pop(id_t id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->force_pop();
    return nullptr;
  }

  aggregated_ptr pop(id_t id)
  {
    if ( auto p = this->get_aggregator(id) )
      return p->pop();
    return nullptr;
  }

  std::string get_name(id_t id) const 
  {
    read_lock<mutex_type> lk(_mutex);
    return _dict.get_name(id);
  }

  id_t create_aggregator(const std::string& name, time_type now)
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
  
  aggregator_ptr create_get_aggregator(const std::string& name, time_type now)
  {
    if ( name.empty() )
      return nullptr;
    std::lock_guard<mutex_type> lk(_mutex);
    id_t id = _dict.create_id( name );
    size_t pos = _dict.id2pos(id);
    if ( pos < _agarr.size() && _agarr[pos]!=nullptr )
      return _agarr[pos];
    
    if ( _agarr.size() <= pos )
      _agarr.resize( pos + 1 );

    if ( _agarr[pos] == nullptr )
    {
      _agarr[pos] = std::make_shared<aggregator_type>(now, _opt, _pool.get_allocator() );
      if ( !_enabled )
        _agarr[pos]->enable(false);
    }
    return _agarr[pos];
  }
  
  aggregator_ptr get_aggregator(id_t id) const
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

  simple_adder_t create_simple_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_simple_adder();
    return nullptr;
  }

  data_adder_t create_data_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_data_adder();
    return nullptr;
  }
  
  reduced_adder_t create_reduced_adder( id_t id )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_reduced_adder();
    return nullptr;
  }
  
  template<typename TH, typename RH, typename WH>
  std::function< void( time_type, time_type, size_type, size_type, size_type) >
    make_composite_handler_(TH time_adder, RH read_adder, WH write_adder, bool summary_size  )
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
  
  composite_adder_t create_composite_adder( id_t time_id, id_t read_id, id_t write_id, bool summary_size )
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
    /*
      [time_adder, read_adder, write_adder]
      (time_type now, time_type span, size_type count, 
       size_type readed, size_type writed)
      {
        if ( time_adder!=nullptr )
          time_adder(now, span, count);
        if ( read_adder!=nullptr )
          read_adder(now, static_cast<time_type>(readed), readed);
        if ( write_adder!=nullptr )
          write_adder(now, static_cast<time_type>(writed), writed);
      };*/
  }

  simple_adder_t create_value_adder(const std::string& name, time_type ts_now)
  {
    return this->create_simple_adder( this->create_aggregator(name, ts_now) );
  }

  data_adder_t create_data_adder(const std::string& name, time_type ts_now)
  {
    return this->create_data_adder( this->create_aggregator(name, ts_now));
  }
  
  reduced_adder_t create_reduced_adder( const std::string& name, time_type ts_now )
  {
    return this->create_reduced_adder(this->create_aggregator(name, ts_now));
  }
  
  composite_adder_t create_composite_adder( 
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
///
  
  std::function<void(aggregated_data::ptr)>
    make_handler_( id_t id, aggregated_handler handler )
  {
    if (id == bad_id || handler==nullptr)
      return nullptr;
    // TDOD: убрать
    return handler;
    /*
    return [id, handler, this]( aggregated_data::ptr ag)
    {
      handler( this->get_name(id), std::move(ag) );
    };
    */
  }
  
  simple_pusher_t create_simple_pusher( id_t id, aggregated_handler handler )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_simple_pusher(make_handler_(id, handler));
    return nullptr;
  }

  data_pusher_t create_data_pusher( id_t id, aggregated_handler handler )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_data_pusher(make_handler_(id, handler));
    return nullptr;
  }
  
  reduced_pusher_t create_reduced_pusher( id_t id, aggregated_handler handler )
  {
    if ( auto ag = this->get_aggregator(id) )
      return ag->create_reduced_pusher(make_handler_(id, handler));
    return nullptr;
  }

  composite_pusher_t create_composite_pusher( 
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
    /*
    return 
      [time_pusher, read_pusher, write_pusher]
      (time_type now, time_type span, size_type count, 
       size_type readed, size_type writed)
      {
        if ( time_pusher!=nullptr )
          time_pusher(now, span, count);
        if ( read_pusher!=nullptr )
          read_pusher(now, static_cast<time_type>(readed), readed);
        if ( write_pusher!=nullptr )
          write_pusher(now, static_cast<time_type>(writed), writed);
      };
      */
  }

//
  simple_pusher_t create_simple_pusher(const std::string& name, aggregated_handler handler, time_type ts_now)
  {
    return this->create_simple_pusher( this->create_aggregator(name, ts_now), handler );
  }

  data_pusher_t create_data_pusher(const std::string& name, aggregated_handler handler, time_type ts_now)
  {
    return this->create_data_pusher( this->create_aggregator(name, ts_now), handler);
  }
  
  reduced_pusher_t create_reduced_pusher( const std::string& name, aggregated_handler handler, time_type ts_now )
  {
    return this->create_reduced_pusher(this->create_aggregator(name, ts_now), handler);
  }
  
  composite_adder_t create_composite_pusher( 
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

  void enable(bool value)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    this->_enabled = value;
    for (auto a : _agarr)
    {
      if ( a!=nullptr )
        a->enable(value);
    }
  }

  bool del(const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);

    id_t id = this->_dict.get_id(name);
    if ( id == static_cast<id_t>(-1) )
      return false;
    size_t pos = _dict.id2pos(id);
    _agarr[ pos ] = nullptr;
    return this->_dict.free(id);
  }
  
private:
  mutable mutex_type _mutex;
  aggregator_options _opt;
  aggregator_list _agarr;
  dict _dict;
  pool<mutex_type> _pool;
  bool _enabled = true;
};

/*
class manager_st: public manager_base<aggregator, empty_mutex>
{
  typedef manager_base<aggregator, empty_mutex> super;
public:
  typedef super::options_type options_type;
  explicit manager_st( const options_type& opt, id_t init, id_t step): manager_base(opt, init, step) {};
};

class manager_mt: public manager_base<aggregator_mt, std::mutex>
{
  typedef manager_base<aggregator_mt, std::mutex> super;
public:
  typedef super::options_type options_type;
  explicit manager_mt( const options_type& opt, id_t init, id_t step): manager_base(opt, init, step) {};
};
*/
}

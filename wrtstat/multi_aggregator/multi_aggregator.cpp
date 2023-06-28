//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2022
//
// Copyright: See COPYING file that comes with this distribution
//

#include "multi_aggregator.hpp"
#include <fas/system/memory.hpp>

namespace wrtstat {

multi_aggregator::multi_aggregator(const options_type& opt )
  : _opt(opt)
{
  this->initialize_();
}

multi_aggregator::multi_aggregator(multi_aggregator&& agh)
  : _hash( std::move(agh._hash) )
  , _opt( std::move(agh._opt) )
  , _mutex_list( std::move(agh._mutex_list) )
  , _aggregator_list( std::move(agh._aggregator_list) )
{
}

multi_aggregator& multi_aggregator::operator=(multi_aggregator&& agh)
{
  _opt = agh._opt;
  _mutex_list = std::move(agh._mutex_list);
  _hash = agh._hash;
  _aggregator_list = std::move(agh._aggregator_list);
  return *this;
}

bool multi_aggregator::push( const std::string& name, const reduced_data& v, push_handler handler)
{
  size_t pos = this->get_pos_(name);
  auto& ag = _aggregator_list[pos];
  const auto& mtx = _mutex_list[pos];
  std::lock_guard<mutex_type> lk( *mtx );
  return ag->push(name, v, std::move(handler) );
}

bool multi_aggregator::push( const request::push& p, const push_handler& handler)
{
  size_t pos = this->get_pos_(p.name);
  auto& ag = _aggregator_list[pos];
  const auto& mtx = _mutex_list[pos];
  std::lock_guard<mutex_type> lk( *mtx );
  return ag->push(p, std::move(handler) );  
}

bool multi_aggregator::multi_push( const request::multi_push& mp, const push_handler& handler)
{
  for ( const request::push& p : mp.data )
    this->push(p, handler);
  return true;
}

bool multi_aggregator::del( const std::string& name)
{
  size_t pos = this->get_pos_(name);
  auto& ag = _aggregator_list[pos];
  const auto& mtx = _mutex_list[pos];
  std::lock_guard<mutex_type> lk( *mtx );
  return ag->del( name );
}


void multi_aggregator::pushout(const push_handler& handler)
{
  size_t size = _mutex_list.size();
  for (size_t i = 0; i < size; ++i)
  {
    const auto& mtx = _mutex_list[i];
    std::lock_guard<mutex_type> lk( *mtx );
    _aggregator_list[i]->pushout(handler);
  }
}

void multi_aggregator::force_pushout(const push_handler& handler)
{
  size_t size = _mutex_list.size();
  for (size_t i = 0; i < size; ++i)
  {
    const auto& mtx = _mutex_list[i];
    std::lock_guard<mutex_type> lk( *mtx );
    _aggregator_list[i]->force_pushout(handler);
  }
}

// ===============================================================

size_t multi_aggregator::get_pos_( const std::string& name) const
{
  return _hash(name) % _opt.hash_size;
}

void multi_aggregator::initialize_()
{
  _mutex_list.resize( _opt.hash_size);
  _aggregator_list.resize( _opt.hash_size);
  std::generate(std::begin(_mutex_list), std::end(_mutex_list), []() noexcept {
    return std::make_unique<mutex_type>();
  });
  std::generate(std::begin(_aggregator_list), std::end(_aggregator_list), [this]() noexcept {
    return std::make_unique<basic_multi_aggregator>(_opt);
  });
}

}

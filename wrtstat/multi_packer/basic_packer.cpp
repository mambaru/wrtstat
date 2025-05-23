//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2021
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wrtstat/api/push_json.hpp>
#include <wrtstat/api/push_json_compact.hpp>
#include <wrtstat/api/multi_push_json.hpp>
#include <wrtstat/api/multi_push_json_compact.hpp>
#include "basic_packer.hpp"
#include <sstream>

#include <iostream>

namespace wrtstat{

basic_packer::basic_packer(const packer_options& opt, const multi_push_handler& handler)
  : _opt(opt)
  , _handler(handler)
{
}

size_t basic_packer::pushout()
{
  if (_handler==nullptr)
    return 0;

  size_t count = 0;
  while ( auto p = this->multi_pop() )
  {
    _handler( std::move(p) );
    ++count;
  }

  if ( !_top.empty() )
  {
    auto mp = std::make_unique<request::multi_push>();
    for ( auto& p : _top )
    {
      mp->data.push_back( std::move(*p.second.first) );
    }
    this->compact( mp.get() );
    _handler( std::move(mp) );
  }

  _top.clear();
  _top.shrink_to_fit();
  return count;
}

bool basic_packer::push( const std::string& name, aggregated_data::ptr ag)
{
  auto req = std::make_unique<request::push>();
  req->name = name;
  static_cast<aggregated_data&>(*req) = std::move(*ag);
  return this->push( std::move(req) );
}

bool basic_packer::push( const request::push& req)
{
  return this->push(std::make_unique<request::push>( req ));
}

bool basic_packer::push( request::push::ptr req)
{
  size_t json_size = this->calc_json_size(*req);
  /*if ( _opt.name_compact )
  {
    size_t pos = 0, count = 0;
    while ( (pos = req->name.find(_opt.name_sep, pos)) != std::string::npos )
    {
      ++count; ++pos;
    }
    // предполагаем, что в среднем каждое имя в списке заменится XX~ примерно, с запасом для >10
    // service~~foo1~~foo2~~foo3 => 0~1~2~3
    json_size -= req->name.size();
    json_size += 3*count;
  }*/
  legend_list_t legend;
  if ( _opt.name_compact )
  {
    std::string name = req->name;
    basic_packer::compact_(&name, &legend, _opt.name_sep);
    json_size -= req->name.size();
    json_size += name.size();
  }

  auto itr = std::lower_bound(_top.begin(), _top.end(), top_pair_t(json_size, push_legend_t()) );
  _top.insert( itr, std::make_pair(json_size, push_legend_t(std::move(req), std::move(legend) ) ) );
  return true;
}

bool basic_packer::multi_push( const request::multi_push& req)
{
  bool status = true;
  for (const request::push& p : req.data)
    status&=this->push(p);
  return status;
}

bool basic_packer::compact(request::multi_push* req)
{
  if ( req->sep.empty() )
    return false;

  for (auto& p : req->data )
  {
    basic_packer::compact_( &p.name, &req->legend, req->sep);
  }
  return true;
}

void basic_packer::compact_(std::string* name, legend_list_t* legend, const std::string& sep)
{
  std::stringstream short_name;
  size_t cur_pos = 0;
  size_t next_pos = name->find(sep);
  std::string item_name = name->substr(cur_pos, next_pos);
  while ( next_pos != std::string::npos)
  {
    short_name << basic_packer::findorcre_(item_name, legend) << "~";
    cur_pos = next_pos + sep.size();
    next_pos = name->find(sep, cur_pos);
    item_name = name->substr(cur_pos, next_pos - cur_pos);
  }
  short_name << basic_packer::findorcre_(item_name, legend);
  *name = short_name.str();
}

basic_packer::name_id_t basic_packer::findorcre_(const std::string& name, legend_list_t* legend)
{
  auto itr = std::find(legend->begin(), legend->end(), name);
  if ( itr != legend->end() )
    return static_cast<basic_packer::name_id_t>( std::distance( legend->begin(), itr) );
  legend->push_back(name);
  return legend->size() - 1;
}

namespace {
struct count_iterator
{
  size_t count = 0;
  count_iterator& operator ++ (int) { ++count; return *this;}
  char& operator*() { return _fake; }
  char _fake=0;
};
}

size_t basic_packer::calc_json_size(const request::push& req, bool compact)
{
  return compact
    ? request::push_json_compact::serializer()(req, count_iterator()).count
    : request::push_json::serializer()(req, count_iterator()).count;
}

size_t basic_packer::calc_json_size(const request::push& req)
{
  return calc_json_size(req, _opt.json_compact);
}

size_t basic_packer::calc_json_size(const request::multi_push& req, bool compact)
{
  return compact
    ? request::multi_push_json_compact::serializer()(req, count_iterator()).count
    : request::multi_push_json::serializer()(req, count_iterator()).count;
}

size_t basic_packer::calc_json_size(const request::multi_push& req)
{
  return calc_json_size(req, _opt.json_compact);
}

void basic_packer::clear()
{
  _top.clear();
}


bool basic_packer::empty() const
{
  return _top.empty();
}

size_t basic_packer::size() const
{
  return _top.size();
}

size_t basic_packer::size(size_t* data_size, size_t* capacity) const
{
  if ( data_size != nullptr )
  {
    for ( auto& p : _top )
    {
      if ( p.second.first!=nullptr)
        *data_size += p.second.first->data.size();
    }
  }

  if ( capacity != nullptr )
  {
    for ( auto& p : _top )
    {
      if ( p.second.first!=nullptr)
        *capacity += p.second.first->data.capacity();
    }
  }

  return this->size();
}


size_t basic_packer::max_val() const
{
  if ( _top.empty() ) return 0;
  return _top.rbegin()->first;
}

size_t basic_packer::min_val() const
{
  if ( _top.empty() ) return 0;
  return _top.begin()->first;
}

request::push::ptr basic_packer::pop_by_json_size(size_t maxsize, size_t* cursize, /*size_t maxdata,*/ legend_list_t* legend)
{
  if ( _top.empty() )
    return nullptr;

  auto itr = std::lower_bound(_top.begin(), _top.end(), top_pair_t(maxsize, push_legend_t() ) );
  if ( itr == _top.end() )
  {
    // любая подходит, берем максимальную
    itr = (++_top.rbegin()).base();
  }
  else if ( itr->first != maxsize )
  {
    // itr указывает на ближайшее большее к maxval
    // берем ближайшее меньшее к maxval, если есть
    if ( itr == _top.begin() )
      return nullptr;
    --itr;
  }

  /* if ( maxdata!=0 && (itr->second.first->data.size() > maxdata) )
    return nullptr;*/

  request::push::ptr p = std::move(itr->second.first);
  if ( cursize!=nullptr)
    *cursize = itr->first;

  if ( legend!=nullptr )
    *legend = std::move(itr->second.second);

  _top.erase(itr);

  return p;
}

/**
 */
request::multi_push::ptr basic_packer::multi_pop()
{
  if ( _top.empty() )
    return nullptr;

  auto res = std::make_unique<request::multi_push>();
  size_t empty_size = this->calc_json_size(*res);
  size_t json_limit = _opt.json_limit;
  size_t data_limit = _opt.data_limit;
  size_t push_limit = _opt.push_limit;
  if (empty_size > json_limit )
    return nullptr;

  std::set<std::string> legend_set;
  json_limit -= empty_size;
  while (json_limit > 0)
  {
    legend_list_t legend;
    size_t cur_json = 0;
    if ( auto p = this->pop_by_json_size(json_limit, &cur_json, &legend) )
    {
      for ( auto& name: legend)
      {
        if ( legend_set.insert(name).second )
          cur_json += name.size() + 4; // "~",
      }
      res->data.push_back(std::move(*p));
      if ( json_limit >= cur_json )
        json_limit -= cur_json;
      else
      {
        // Возвращаем обратно
        this->push( std::move(p) );
        break;
      }

      if ( json_limit > 0 )
        json_limit -= static_cast<size_t>(_opt.json_limit!=0);

      if ( _opt.push_limit != 0)
      {
        --push_limit;
        if ( push_limit==0 )
          break;
      }

      if ( _opt.data_limit != 0 )
      {
        if ( p->data.size() > data_limit )
        {
          p->data.resize(data_limit);
          break;
        }
        data_limit -= p->data.size();
        if ( data_limit==0 )
          break;
      }
    }
    else
      break;
  }

  if ( res->data.empty() )
    return nullptr;

  res->sep = _opt.name_sep;

  if ( _opt.name_compact )
    basic_packer::compact(res.get());
  return res;
}

bool basic_packer::recompact(request::multi_push* req, std::string* err)
{
  if ( req->legend.empty() )
    return true;

  if ( req->sep.empty() )
  {
    if (err!=nullptr) *err = "Separator not specified";
    return false;
  }

  for (auto& p : req->data)
  {
    std::stringstream ss;
    auto beg = p.name.begin();
    auto end = p.name.end();
    while (beg!=end)
    {
      if ( !wjson::parser::is_number(beg, end) )
      {
        if (err!=nullptr) *err = "Invalid compact name format";
        return false;
      }
      size_t pos = 0;
      beg = wjson::parser::unserialize_integer(pos, beg, end, nullptr);
      if ( pos >= req->legend.size() )
      {
        if (err!=nullptr) *err = "Invalid legend or compact name format";
        return false;
      }

      ss << req->legend[pos];
      if ( beg == end )
        break;

      if ( *beg!='~')
      {
        if (err!=nullptr) *err = "Invalid compact name format (~ expected)";
        return false;
      }
      ++beg;
      ss << req->sep;
    }
    p.name = ss.str();
  }
  req->legend.clear();
  return true;
}

}

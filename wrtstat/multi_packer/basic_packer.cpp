//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
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
  if ( _opt.name_compact )
    _legend.reserve(128);
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
  if ( _opt.name_compact )
  {
    this->compact_( req.get()) ;
  }
  size_t json_size = this->calc_json_size(*req);
  _top.insert( std::make_pair(json_size, std::move(req)));
  return true;
}


bool basic_packer::multi_push( const request::multi_push& req)
{
  bool status = true;
  for (const request::push& p : req.data)
    status&=this->push(p);
  return status;
}

bool basic_packer::recompact(
  std::string* out_name,
  std::set<std::string>* out_legend,
  const std::string& name,
  const std::string& sep,
  const legend_list_t& legend,
  std::string* err
)
{
  if ( legend.empty() )
    return true;

  std::stringstream ss;

  auto beg = name.begin();
  auto end = name.end();
  while (beg!=end)
  {
    if ( !wjson::parser::is_number(beg, end) )
    {
      if (err!=nullptr) *err = "Invalid compact name format";
      return false;
    }
    size_t pos = 0;
    beg = wjson::parser::unserialize_integer(pos, beg, end, nullptr);
    if ( pos >= legend.size() )
    {
      if (err!=nullptr) *err = "Invalid legend or compact name format";
      return false;
    }
    if ( out_name != nullptr)
      ss << legend[pos];
    if ( out_legend!= nullptr )
      out_legend->insert(legend[pos]);
    if ( beg == end )
      break;
    if ( *beg!='~')
    {
      if (err!=nullptr) *err = "Invalid compact name format (~ expected)";
      return false;
    }
    ++beg;
    if ( out_name != nullptr)
      ss << sep;
  }

  if ( out_name != nullptr)
    *out_name = ss.str();
  return true;

}


bool basic_packer::recompact(request::push* req, const std::string& sep, const legend_list_t& legend, std::string* err)
{
  auto name = std::move(req->name);
  return basic_packer::recompact(&(req->name), nullptr, name, sep, legend, err);
  /*
  if ( legend.empty() )
    return true;

  std::stringstream ss;

  auto beg = req->name.begin();
  auto end = req->name.end();
  while (beg!=end)
  {
    //std::cout << std::endl << std::string(beg, end) << std::endl;
    if ( !wjson::parser::is_number(beg, end) )
    {
      if (err!=nullptr) *err = "Invalid compact name format";
      return false;
    }
    size_t pos = 0;
    beg = wjson::parser::unserialize_integer(pos, beg, end, nullptr);
    if ( pos >= legend.size() )
    {
      if (err!=nullptr) *err = "Invalid legend or compact name format";
      return false;
    }
    ss << legend[pos];
    if ( beg == end )
      break;
    if ( *beg!='~')
    {
      if (err!=nullptr) *err = "Invalid compact name format (~ expected)";
      return false;
    }
    ++beg;
    ss << sep;
  }
  req->name = ss.str();
  return true;*/
}

bool basic_packer::recompact(request::push* req, std::string* err)
{
  return basic_packer::recompact(req, this->_opt.name_sep, this->legend(), err);
}

bool basic_packer::recompact(request::multi_push* req, std::string* err)
{
  for (auto& p : req->data )
    if ( !basic_packer::recompact(&p, req->sep, req->legend, err) )
      return false;
  return true;
}

const basic_packer::legend_list_t& basic_packer::legend() const
{
  return _legend;
}

bool basic_packer::compact(request::push* req)
{
  if ( !_opt.name_compact )
    return false;
  this->compact_(req);
  return true;
}

void basic_packer::compact_( request::push* req)
{
  std::stringstream short_name;
  size_t cur_pos = 0;
  size_t next_pos = req->name.find(_opt.name_sep);
  std::string item_name = req->name.substr(cur_pos, next_pos);
  while ( next_pos != std::string::npos)
  {
    short_name << findorcre_(item_name) << "~";
    cur_pos = next_pos + _opt.name_sep.size();
    next_pos = req->name.find(_opt.name_sep, cur_pos);
    item_name = req->name.substr(cur_pos, next_pos - cur_pos);
  }
  short_name << findorcre_(item_name);
  req->name = short_name.str();
}

basic_packer::name_id_t basic_packer::findorcre_(const std::string& name)
{
  //std::cout << "\n[" << name << "]" << std::endl;
  auto itr = _name_id_map.find(name);
  if ( itr != _name_id_map.end() )
    return itr->second;
  name_id_t id = _legend.size();
  _name_id_map.insert( std::make_pair(name, id) );
  _legend.push_back(name);
  return id;
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
  _legend.clear();
  _name_id_map.clear();
}


bool basic_packer::empty() const
{
  return _top.empty();
}

bool basic_packer::size() const
{
  return _top.size();
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

request::push::ptr basic_packer::pop_by_json_size(size_t maxsize, size_t* cursize, size_t maxdata)
{
  if ( _top.empty() )
    return nullptr;

  auto itr = _top.lower_bound(maxsize);
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

  if ( maxdata!=0 && (itr->second->data.size() > maxdata) )
    return nullptr;

  request::push::ptr p = std::move(itr->second);
  if ( cursize!=nullptr)
    *cursize = itr->first;
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
  json_limit -= empty_size;
  while (json_limit > 0)
  {
    size_t cur_json = 0;
    if ( auto p = this->pop_by_json_size(json_limit, &cur_json, data_limit) )
    {
      res->data.push_back(std::move(*p));
      if ( json_limit >= cur_json )
        json_limit -= cur_json;
      else
        abort();
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
          abort();
          //break;
        }
        data_limit -= p->data.size();
        if ( data_limit==0 )
          break;
      }
    }
    else
      break;
  }

  std::set<std::string> name_legend;

  for (const auto& d: res->data)
  {
    recompact(nullptr, &name_legend, d.name, res->sep, _legend, nullptr);

  }
  res->legend.assign(name_legend.begin(), name_legend.end());

  return res;
}


}

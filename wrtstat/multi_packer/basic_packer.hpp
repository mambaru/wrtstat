//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/api/push.hpp>
#include <wrtstat/api/multi_push.hpp>
#include <wrtstat/aggregator/api/reduced_data.hpp>
#include <wrtstat/multi_packer/packer_options.hpp>
#include <fas/system/memory.hpp>
#include <unordered_map>
#include <map>
#include <set>
#include <limits>

namespace wrtstat{

class basic_packer
{
public:
  typedef size_t name_id_t;
  typedef std::vector<std::string> legend_list_t;
  typedef std::unordered_map<std::string, name_id_t> name_id_map_t;

  typedef std::function<void(request::multi_push::ptr)> multi_push_handler;
  basic_packer(const packer_options& opt, const multi_push_handler& handler);


  // return true если сработал обработчик
  bool push( const std::string& name, aggregated_data::ptr ag);
  bool push( const request::push& req);
  bool push( request::push::ptr req);
  bool multi_push( const request::multi_push& req);

  request::push::ptr pop_by_json_size(size_t maxsize, size_t* cursize, size_t maxdata);
  request::multi_push::ptr multi_pop();

  size_t pushout();

  bool empty() const;
  bool size() const;
  size_t max_val() const;
  size_t min_val() const;

  size_t calc_json_size(const request::push& req);
  size_t calc_json_size(const request::multi_push& req);
  const legend_list_t& legend() const;
  bool compact(request::push* req);
  void clear();
  bool recompact(request::push* req, std::string* err);

  static bool recompact(
    std::string* out_name, std::set<std::string>* out_legend,
    const std::string& name,
    const std::string& sep, const legend_list_t& legend, std::string* err);
  static bool recompact(request::push* req, const std::string& sep, const legend_list_t& legend, std::string* err);
  static bool recompact(request::multi_push* req, std::string* err);
  static size_t calc_json_size(const request::push& req, bool compact);
  static size_t calc_json_size(const request::multi_push& req, bool compact);

private:
  void compact_( request::push* req);
  name_id_t findorcre_(const std::string& name);
private:
  packer_options _opt;
  multi_push_handler _handler;

  legend_list_t _legend;
  name_id_map_t _name_id_map;
  //push_collector _collector;
  size_t _counter = std::numeric_limits<size_t>::max();
  std::multimap<size_t, request::push::ptr> _top;
};

}

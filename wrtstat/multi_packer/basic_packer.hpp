//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016-2021
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
  typedef std::vector<std::string> legend_list_t;
  typedef std::pair<request::push::ptr, legend_list_t> push_legend_t;
  typedef std::pair<size_t, push_legend_t> top_pair_t;
  typedef std::vector<top_pair_t> top_list_t;

public:
  typedef size_t name_id_t;

  typedef std::function<void(request::multi_push::ptr)> multi_push_handler;
  basic_packer(const packer_options& opt, const multi_push_handler& handler);

  // return true если сработал обработчик
  bool push( const std::string& name, aggregated_data::ptr ag);
  bool push( const request::push& req);
  bool push( request::push::ptr req);
  bool multi_push( const request::multi_push& req);

  request::push::ptr pop_by_json_size(size_t maxsize, size_t* cursize, /*size_t maxdata,*/ legend_list_t* legend);
  request::multi_push::ptr multi_pop();

  size_t pushout();

  bool empty() const;
  size_t size() const;
  size_t size(size_t* data_size) const;

  size_t max_val() const;
  size_t min_val() const;

  size_t calc_json_size(const request::push& req);
  size_t calc_json_size(const request::multi_push& req);
  static bool compact(request::multi_push* req);
  static bool recompact(request::multi_push* req, std::string* err);
  void clear();
  static size_t calc_json_size(const request::push& req, bool compact);
  static size_t calc_json_size(const request::multi_push& req, bool compact);

private:
  static void compact_(std::string* name, legend_list_t* legend, const std::string& sep);
  static name_id_t findorcre_(const std::string& name, legend_list_t* legend);
private:
  packer_options _opt;
  multi_push_handler _handler;

  top_list_t _top;
  //std::multimap<size_t, request::push::ptr> _top;
};

}

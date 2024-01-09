#include <wrtstat/multi_packer/basic_packer.hpp>
#include <wrtstat/api/multi_push_json_compact.hpp>
#include <wrtstat/api/push_json_compact.hpp>
#include <wrtstat/api/multi_push_json.hpp>
#include <fas/testing.hpp>

namespace {

UNIT(packer_recompact1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  request::multi_push req;
  req.sep = "~~";
  req.data.resize(2);
  req.legend={"name1", "name2", "name3"};
  req.data[0].name="0~1~2";
  req.data[1].name="2~1~0";
  basic_packer::recompact(&req, nullptr);
  t << equal<expect, std::string>("name1~~name2~~name3", req.data[0].name) << FAS_FL;
  t << equal<expect, std::string>("name3~~name2~~name1", req.data[1].name) << FAS_FL;
}

UNIT(packer_recompact2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  request::multi_push req;
  req.sep = "~~";
  req.data.resize(3);
  req.legend={"name", "name3", "name2", " name ", "name1", "xname"};
  req.data[0].name="1~2~4";
  req.data[1].name="4~2~1";
  req.data[2].name="5~4~3~2~1~0";
  basic_packer::recompact(&req, nullptr);
  t << equal<expect, std::string>("name3~~name2~~name1", req.data[0].name) << FAS_FL;
  t << equal<expect, std::string>("name1~~name2~~name3", req.data[1].name) << FAS_FL;
  t << equal<expect, std::string>("xname~~name1~~ name ~~name2~~name3~~name", req.data[2].name) << FAS_FL;
}

UNIT(packer_compact1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  request::multi_push req;
  req.sep = "~~";
  req.data.resize(1);
  req.data[0].name="";
  basic_packer::compact(&req);
  t << equal<expect, std::string>("0", req.data[0].name) << FAS_FL;
  t << equal<assert, size_t>(1, req.legend.size()) << FAS_FL;
  t << stop;

  req.legend.clear();
  req.data[0].name="name1";
  basic_packer::compact(&req);
  t << equal<expect, std::string>("0", req.data[0].name) << FAS_FL;
  t << equal<assert, size_t>(1, req.legend.size()) << FAS_FL;
  t << stop;

  req.legend.clear();
  req.data[0].name="name1~~name2~~name3";
  basic_packer::compact(&req);
  t << equal<expect, std::string>("0~1~2", req.data[0].name) << FAS_FL;
  t << equal<assert, size_t>(3, req.legend.size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("name1", req.legend[0]) << FAS_FL;
  t << equal<expect, std::string>("name2", req.legend[1]) << FAS_FL;
  t << equal<expect, std::string>("name3", req.legend[2]) << FAS_FL;

  req.legend.clear();
  req.data[0].name="~~name1~~name2~~name3~~";
  basic_packer::compact(&req);
  t << equal<expect, std::string>("0~1~2~3~0", req.data[0].name) << FAS_FL;
  t << equal<assert, size_t>(4, req.legend.size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", req.legend[0]) << FAS_FL;
  t << equal<expect, std::string>("name1", req.legend[1]) << FAS_FL;
  t << equal<expect, std::string>("name2", req.legend[2]) << FAS_FL;
  t << equal<expect, std::string>("name3", req.legend[3]) << FAS_FL;

  req.legend.clear();
  req.data[0].name="~~name~1~~name~2~~name~3~~";
  basic_packer::compact(&req);
  t << equal<expect, std::string>("0~1~2~3~0", req.data[0].name) << FAS_FL;
  t << equal<assert, size_t>(4, req.legend.size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", req.legend[0]) << FAS_FL;
  t << equal<expect, std::string>("name~1", req.legend[1]) << FAS_FL;
  t << equal<expect, std::string>("name~2", req.legend[2]) << FAS_FL;
  t << equal<expect, std::string>("name~3", req.legend[3]) << FAS_FL;

  req.legend.clear();
  req.data[0].name="~~~~~~~";
  basic_packer::compact(&req);
  t << equal<expect, std::string>("0~0~0~1", req.data[0].name) << FAS_FL;
  t << equal<assert, size_t>(2, req.legend.size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", req.legend[0]) << FAS_FL;
  t << equal<expect, std::string>("~", req.legend[1]) << FAS_FL;
}

UNIT(push_top1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  packer_options opt;
  opt.json_compact = true;
  opt.name_compact = false;
  basic_packer pt(opt, nullptr);
  request::push req;
  req.name="1";
  pt.push(req);
  req.name="22";
  pt.push(req);
  req.name="333";
  pt.push(req);

  t << message("MAX_SIZE=") << pt.max_val();
  request::push::ptr res = pt.pop_by_json_size(32, nullptr, nullptr);
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>(res->name, "333") << FAS_FL;
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  res = pt.pop_by_json_size(31, nullptr, nullptr);
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>(res->name, "22") << FAS_FL;
  res = pt.pop_by_json_size(30, nullptr, nullptr);
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>(res->name, "1") << FAS_FL;
}

UNIT(basic_packer1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  packer_options opt;
  opt.json_limit = 512;
  opt.push_limit = 10;
  opt.data_limit = 10;
  opt.json_compact = true;
  opt.name_compact = false;
  size_t total1 = 0;
  size_t total2 = 0;
//  size_t total4 = 0;
  // &t, &total1, &total2,
  basic_packer pt(opt, [&](request::multi_push::ptr req)
  {
    size_t values = 0;
    for ( const auto &p : req->data)
      values += p.data.size();
    t << message("data count=") << req->data.size() << " json=" << pt.calc_json_size(*req) << " values=" << values;
    total1 += req->data.size();
    ++total2;
//    total4+=values;
  });
  for (int i = 0; i < 1000; ++i)
  {
    auto req = std::make_unique<request::push>();
    req->name="name"+std::to_string(i);
    size_t count = 1 + size_t(std::rand() % 9);
    for ( size_t c = 0; c < count; ++c)
      req->data.push_back(std::rand());
    pt.push(std::move(req));
  }

  t << is_false<expect>( pt.empty()) << FAS_FL;
  size_t total3 = 0;
  while (size_t count = pt.pushout() )
  {
    total3+=count;
  }
  t << is_true<expect>( pt.empty()) << FAS_FL;
  t << equal<expect, size_t>(total1, 1000) << FAS_FL;
  t << equal<expect, size_t>(total2, total3) << FAS_FL;
}

UNIT(basic_packer2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  packer_options opt;
  opt.json_limit = 1024;
  opt.push_limit = 0;
  opt.data_limit = 0;
  opt.json_compact = true;
  opt.name_compact = true;
  size_t total1 = 0;
 // size_t total2 = 0;
//  size_t total4 = 0;
  // &t, &total1, &total2,
  basic_packer pt(opt, nullptr);

  for (int i = 0; i < 1000; ++i)
  {
    ++total1;
    auto req = std::make_unique<request::push>();
    req->name="service~~server~~name"+std::to_string(i);
    size_t count = 1 + size_t(std::rand() % 9);
    for ( size_t c = 0; c < count; ++c)
      req->data.push_back(std::rand());
    pt.push(std::move(req));
  }

  t << is_false<expect>( pt.empty()) << FAS_FL;
  while (auto req = pt.multi_pop() )
  {
    //t << equal<expect, size_t>(req->data.size(), opt.push_limit) << FAS_FL;

    //total2+=req->data.size();

    std::string resjson;
    request::multi_push_json_compact::serializer()( *req, std::back_inserter(resjson));

    std::string resjson2;
    basic_packer::recompact(req.get(), nullptr);
    request::multi_push_json_compact::serializer()( *req, std::back_inserter(resjson2));

    std::string resjson3;
    request::push_json_compact::serializer()( req->data[0], std::back_inserter(resjson3));

    t << less_equal<expect, size_t>(resjson.size(), 1024) << ": \n"
      << resjson  << "\n"
      << resjson2 << "\n"
      << resjson3 << "\n"
      << FAS_FL;
  }
  t << is_true<expect>( pt.empty()) << FAS_FL;
  t << equal<expect, size_t>(total1, 1000) << FAS_FL;
 // t << equal<expect, size_t>(total1, total2) << FAS_FL;
}


} // namespace

BEGIN_SUITE(packer, "")
  ADD_UNIT(packer_recompact1)
  ADD_UNIT(packer_recompact2)
  ADD_UNIT(packer_compact1)
  ADD_UNIT(push_top1)
  ADD_UNIT(basic_packer1)
  ADD_UNIT(basic_packer2)

END_SUITE(packer)


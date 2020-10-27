#include <wrtstat/multi_packer/basic_packer.hpp>
#include <fas/testing.hpp>

namespace {

UNIT(packer_compact1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  std::string err;
  packer_options opt;
  opt.name_compact = true;
  basic_packer p(opt, nullptr);
  request::push req;

  req.name="";
  p.compact(&req);
  t << equal<expect, std::string>("0", req.name) << FAS_FL;
  t << equal<assert, size_t>(1, p.legend().size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", p.legend()[0]) << FAS_FL;
  t << is_true<expect>(p.recompact(&req, &err)) << err << FAS_FL;
  t << equal<expect, std::string>("", req.name) << FAS_FL;
  p.clear();

  req.name="name1";
  p.compact(&req);
  t << equal<expect, std::string>("0", req.name) << FAS_FL;
  t << equal<assert, size_t>(1, p.legend().size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("name1", p.legend()[0]) << FAS_FL;
  t << is_true<expect>(p.recompact(&req, &err)) << err << FAS_FL;
  t << equal<expect, std::string>("name1", req.name) << FAS_FL;
  p.clear();

  req.name="name1~~name2~~name3";
  p.compact(&req);
  t << equal<expect, std::string>("0~1~2", req.name) << FAS_FL;
  t << equal<assert, size_t>(3, p.legend().size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("name1", p.legend()[0]) << FAS_FL;
  t << equal<expect, std::string>("name2", p.legend()[1]) << FAS_FL;
  t << equal<expect, std::string>("name3", p.legend()[2]) << FAS_FL;
  t << is_true<expect>(p.recompact(&req, &err)) << err << FAS_FL;
  t << equal<expect, std::string>("name1~~name2~~name3", req.name) << FAS_FL;
  p.clear();

  req.name="~~name1~~name2~~name3~~";
  p.compact(&req);
  t << equal<expect, std::string>("0~1~2~3~0", req.name) << FAS_FL;
  t << equal<assert, size_t>(4, p.legend().size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", p.legend()[0]) << FAS_FL;
  t << equal<expect, std::string>("name1", p.legend()[1]) << FAS_FL;
  t << equal<expect, std::string>("name2", p.legend()[2]) << FAS_FL;
  t << equal<expect, std::string>("name3", p.legend()[3]) << FAS_FL;
  t << is_true<expect>(p.recompact(&req, &err)) << err << FAS_FL;
  t << equal<expect, std::string>("~~name1~~name2~~name3~~", req.name) << FAS_FL;
  p.clear();

  req.name="~~name~1~~name~2~~name~3~~";
  p.compact(&req);
  t << equal<expect, std::string>("0~1~2~3~0", req.name) << FAS_FL;
  t << equal<assert, size_t>(4, p.legend().size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", p.legend()[0]) << FAS_FL;
  t << equal<expect, std::string>("name~1", p.legend()[1]) << FAS_FL;
  t << equal<expect, std::string>("name~2", p.legend()[2]) << FAS_FL;
  t << equal<expect, std::string>("name~3", p.legend()[3]) << FAS_FL;
  t << is_true<expect>(p.recompact(&req, &err)) << err << FAS_FL;
  t << equal<expect, std::string>("~~name~1~~name~2~~name~3~~", req.name) << FAS_FL;
  p.clear();

  req.name="~~~~~~~";
  p.compact(&req);
  t << equal<expect, std::string>("0~0~0~1", req.name) << FAS_FL;
  t << equal<assert, size_t>(2, p.legend().size()) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>("", p.legend()[0]) << FAS_FL;
  t << equal<expect, std::string>("~", p.legend()[1]) << FAS_FL;
  t << is_true<expect>(p.recompact(&req, &err)) << err << FAS_FL;
  t << equal<expect, std::string>("~~~~~~~", req.name) << FAS_FL;
  p.clear();
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
  request::push::ptr res = pt.pop_by_json_size(32, nullptr,0);
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>(res->name, "333") << FAS_FL;
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  res = pt.pop_by_json_size(31, nullptr, 0);
  t << message("MAX_SIZE=") << pt.max_val();
  t << is_true<assert>(res!=nullptr) << FAS_FL;
  t << stop;
  t << equal<expect, std::string>(res->name, "22") << FAS_FL;
  res = pt.pop_by_json_size(30, nullptr, 0);
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
  size_t total4 = 0;
  // &t, &total1, &total2, 
  basic_packer pt(opt, [&](request::multi_push::ptr req)
  {
//#warning TODO проверить размер JSON
    size_t values = 0;
    for ( const auto &p : req->data)
      values += p.data.size();
    t << message("data count=") << req->data.size() << " json=" << pt.calc_json_size(*req) << " values=" << values;
    total1 += req->data.size();
    ++total2;
    total4+=values;
  });
  for (int i = 0; i < 1000; ++i)
  {
    auto req = std::make_unique<request::push>();
    req->name="name"+std::to_string(i);
    size_t count = 1 + std::rand() % 9;
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
  
} // namespace

BEGIN_SUITE(packer, "")
  ADD_UNIT(packer_compact1)
  ADD_UNIT(push_top1)
  ADD_UNIT(basic_packer1)
END_SUITE(packer)


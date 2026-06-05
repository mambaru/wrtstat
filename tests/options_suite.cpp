#include <fas/testing.hpp>
#include <wrtstat/options/load_options.hpp>
#include <fstream>
#include <cstdio>

namespace {

const char* valid_json =
  R"({"reducer_limit":1024,"reducer_levels":128,"aggregation_step_ts":10)"
  R"(,"soiled_start_ts":0,"resolution":"none","outgoing_reduced_size":64,"pool_size":8})";

UNIT(load_options_inline, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  wrtstat_options opt;
  std::string err;
  t << is_true<assert>( load_options(valid_json, &opt, &err) ) << FAS_FL;
  t << equal<expect, size_t>( opt.reducer_limit, 1024ul ) << FAS_FL;
  t << equal<expect, size_t>( opt.reducer_levels, 128ul ) << FAS_FL;
  t << equal<expect, time_type>( opt.aggregation_step_ts, 10 ) << FAS_FL;
  t << equal<expect, size_t>( opt.outgoing_reduced_size, 64ul ) << FAS_FL;
  t << equal<expect, size_t>( opt.pool_size, 8ul ) << FAS_FL;
  t << equal<expect>( opt.resolution, resolutions::none ) << FAS_FL;
  t << nothing;
}

UNIT(load_options_file, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  const std::string path = "/tmp/wrtstat_load_options_test.conf";
  {
    std::ofstream ofs(path);
    ofs << valid_json;
  }

  wrtstat_options opt;
  std::string err;
  t << is_true<assert>( load_options(path, &opt, &err) ) << FAS_FL;
  t << equal<expect, size_t>( opt.reducer_limit, 1024ul ) << FAS_FL;
  std::remove(path.c_str());
  t << nothing;
}

UNIT(load_options_errors, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  wrtstat_options opt;
  std::string err;

  t << is_false<assert>( load_options("/tmp/wrtstat_no_such_file_xxx.conf", &opt, &err) ) << FAS_FL;
  t << is_false<assert>( err.empty() ) << FAS_FL;

  t << is_false<assert>( load_options(R"({"reducer_limit":"bad"})", &opt, &err) ) << FAS_FL;
  t << is_false<assert>( err.empty() ) << FAS_FL;

  t << nothing;
}

UNIT(serialize_options_roundtrip, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  wrtstat_options opt;
  opt.reducer_limit = 512;
  opt.reducer_levels = 64;
  opt.aggregation_step_ts = 100;
  opt.resolution = resolutions::microseconds;
  opt.outgoing_reduced_size = 32;
  opt.pool_size = 16;

  const std::string json = serialize_options(opt);
  wrtstat_options loaded;
  std::string err;
  t << is_true<assert>( load_options(json, &loaded, &err) ) << FAS_FL;
  t << equal<expect, size_t>( loaded.reducer_limit, opt.reducer_limit ) << FAS_FL;
  t << equal<expect, size_t>( loaded.reducer_levels, opt.reducer_levels ) << FAS_FL;
  t << equal<expect, time_type>( loaded.aggregation_step_ts, opt.aggregation_step_ts ) << FAS_FL;
  t << equal<expect, size_t>( loaded.outgoing_reduced_size, opt.outgoing_reduced_size ) << FAS_FL;
  t << equal<expect, size_t>( loaded.pool_size, opt.pool_size ) << FAS_FL;
  t << equal<expect>( loaded.resolution, opt.resolution ) << FAS_FL;

  auto by_value = load_options(json, &err);
  t << equal<expect, size_t>( by_value.reducer_limit, opt.reducer_limit ) << FAS_FL;
  t << nothing;
}

}

BEGIN_SUITE(options, "")
  ADD_UNIT(load_options_inline)
  ADD_UNIT(load_options_file)
  ADD_UNIT(load_options_errors)
  ADD_UNIT(serialize_options_roundtrip)
END_SUITE(options)

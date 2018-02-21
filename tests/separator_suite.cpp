#include <fas/testing.hpp>
#include <wrtstat/separator.hpp>

namespace {
  
UNIT(separator0, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  t << equal<expect, time_type>( std::time(0), separator::now_t<std::chrono::seconds>() ) << FAS_FL;
  
  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = 0;
  opt.aggregation_step_ts = 10;
  separator sep(0, opt);
  for (int i = 0 ; i < 100; i++)
  {
    t << is_true<expect>( sep.add(i, i, 1) ) << "i=" << i  << FAS_FL;
    t << equal<expect>( sep.current_time(), (i/10)*10 ) << "i=" << i << FAS_FL;
    t << equal<expect>( sep.next_time(), (i/10)*10 + 10 ) << "i=" << i << FAS_FL;
  }
  sep.separate(100, true);
  t << equal<expect>( sep.size(), 10ul ) << FAS_FL;
  t << is_true<expect>( sep.ready() ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    auto s = sep.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, i*10 ) << "i=" << i << FAS_FL;
  }

  t << nothing;
}

UNIT(separator1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  t << equal<expect, time_type>( std::time(0), separator::now_t<std::chrono::seconds>() ) << FAS_FL;
  
  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = 0;
  opt.aggregation_step_ts = 10;
  separator sep(0, opt);
  for (int i = 10 ; i < 110; i++)
  {
    t << is_true<expect>( sep.add(i, i, 1) ) << "i=" << i  << FAS_FL;
    t << equal<expect>( sep.current_time(), (i/10)*10 ) << "i=" << i << FAS_FL;
    t << equal<expect>( sep.next_time(), (i/10)*10 + 10 ) << "i=" << i << FAS_FL;
  }
  sep.separate(110, true);
  t << equal<expect>( sep.size(), 10ul ) << FAS_FL;
  t << is_true<expect>( sep.ready() ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    auto s = sep.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, (i+1)*10 ) << "i=" << i << FAS_FL;
  }

  t << nothing;
}

UNIT(separator2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  t << equal<expect, time_type>( std::time(0)*1000000000, (separator::now_t<std::chrono::nanoseconds>()/1000000000 * 1000000000) ) << FAS_FL;
  
  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = 1000000000;
  opt.aggregation_step_ts = 1000;
  separator sep(0, opt);
  for (int j = 0 ; j < 10; ++j)
  {
    for (int i = 0 ; i < 100; i++)
    {
      auto now = separator::now_t<std::chrono::nanoseconds>();
      t << is_true<expect>( sep.add(now, i, 1) ) << "i=" << i  << FAS_FL;
      t << equal<expect>( sep.current_time(), (now/opt.aggregation_step_ts)*opt.aggregation_step_ts ) << "i=" << i << FAS_FL;
      t << equal<expect>( sep.next_time(), (now/opt.aggregation_step_ts + 1)*opt.aggregation_step_ts ) << "i=" << i << FAS_FL;
    }
  }
  sep.separate(0, true);
  t << equal<expect>( sep.size(), 1000ul ) << FAS_FL;
  t << is_true<expect>( sep.ready() ) << FAS_FL;
  for (int i = 0 ; i < 1000ul; i++)
  {
    auto s = sep.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
  }

  t << nothing;
}
}

BEGIN_SUITE(separator, "")
  ADD_UNIT(separator0)
  ADD_UNIT(separator1)
  ADD_UNIT(separator2)
END_SUITE(separator)


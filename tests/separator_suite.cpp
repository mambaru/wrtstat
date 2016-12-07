#include <fas/testing.hpp>
#include <wrtstat/separator.hpp>

namespace {

UNIT(separator1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  t << equal<expect, types::time_type>( std::time(0), separator::now<std::chrono::seconds>() ) << FAS_FL;
  
  separator_options opt;
  opt.levels = 1;
  opt.limit  = 8;
  opt.step_ts = 10;
  separator sep(0, opt);
  for (int i = 0 ; i < 100; i++)
  {
    t << is_true<expect>( sep.add(i, i, 1) ) << FAS_FL;
    t << equal<expect>( sep.current_time(), (i/10)*10 ) << "i=" << FAS_FL;
    t << equal<expect>( sep.next_time(), (i/10)*10 + 10 ) << "i=" << FAS_FL;
  }
  sep.separate(100, true);
  t << equal<expect>( sep.size(), 10 ) << FAS_FL;
  t << is_true<expect>( sep.ready() ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    auto s = sep.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, i*10 ) << "i=" << FAS_FL;
  }

  t << nothing;
}

}

BEGIN_SUITE(separator, "")
  ADD_UNIT(separator1)
END_SUITE(separator)


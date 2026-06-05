#include <fas/testing.hpp>
#include <wrtstat/aggregator/separator.hpp>
#include <thread>

namespace {
  
UNIT(separator0, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  t << equal<expect, time_type>( std::time(nullptr), separator::now_t<std::chrono::seconds>() ) << FAS_FL;
  
  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;
  separator sep(0, opt);
  time_t tc = sep.current_time();
  t << equal<expect>( tc, 0 ) << FAS_FL;
  for (int i = 0 ; i < 100; i++)
  {
    t << is_true<expect>( sep.add(i, i, 1) ) << "i=" << i  << FAS_FL;
    t << equal<expect>( sep.current_time(), (i/10)*10 ) << "i=" << i << FAS_FL;
    t << equal<expect>( sep.next_time(), (i/10)*10 + 10 ) << "i=" << i << FAS_FL;
  }
  sep.separate(100, nullptr, true);
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

  t << equal<expect, time_type>( std::time(nullptr), separator::now_t<std::chrono::seconds>() ) << FAS_FL;
  
  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;
  separator sep(0, opt);
  for (int i = 10 ; i < 110; i++)
  {
    t << is_true<expect>( sep.add(i, i, 1) ) << "i=" << i  << FAS_FL;
    t << equal<expect>( sep.current_time(), (i/10)*10 ) << "i=" << i << FAS_FL;
    t << equal<expect>( sep.next_time(), (i/10)*10 + 10 ) << "i=" << i << FAS_FL;
  }
  sep.separate(110, nullptr, true);
  t << equal<expect>( sep.size(), 10ul ) << FAS_FL;
  t << is_true<expect>( sep.ready() ) << FAS_FL;
  for (int i = 0 ; i < 10; i++)
  {
    auto s = sep.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
    t << equal<expect>( s->ts, (i+1)*10 ) << "i=" << i << FAS_FL;
  }
}

UNIT(separator2, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  t << equal<expect, time_type>( std::time(nullptr)*1000000000, (separator::now_t<std::chrono::nanoseconds>()/1000000000 * 1000000000) ) 
    << FAS_FL;
  
  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = resolutions::nanoseconds;
  opt.aggregation_step_ts = 1000;
  separator sep(0, opt);
  time_t now_stub = time(nullptr);
  for (int j = 0 ; j < 10; ++j)
  {
    for (int i = 0 ; i < 100; i++)
    {
      if ( time(nullptr) > now_stub + 1  )
      {
        t << warning("Сработала заглушка. Под valgrind?");
        return; // заглушка для valgrind( очень долго )
      }
      auto now = separator::now_t<std::chrono::nanoseconds>();
      t << is_true<expect>( sep.add(now, i, 1) ) << "i=" << i  << FAS_FL;
      t << equal<expect>( sep.current_time(), (now/opt.aggregation_step_ts)*opt.aggregation_step_ts ) << "i=" << i << FAS_FL;
      t << equal<expect>( sep.next_time(), (now/opt.aggregation_step_ts + 1)*opt.aggregation_step_ts ) << "i=" << i << FAS_FL;
      std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    }
  }

  t << flush;
  sep.separate(0, nullptr, true);
  t << equal<expect>( sep.size(), 1000ul ) << FAS_FL;
  t << is_true<expect>( sep.ready() ) << FAS_FL;
  for (int i = 0 ; i < 1000; i++)
  {
    auto s = sep.pop();
    t << is_true<assert>( s!=nullptr ) << "i=" << i << FAS_FL;
    t << stop;
  }

  t << nothing;
}

UNIT(separator_stale_ts, "")
{
  using namespace fas::testing;
  using namespace wrtstat;

  separator_options opt;
  opt.reducer_levels = 1;
  opt.reducer_limit  = 8;
  opt.resolution = resolutions::none;
  opt.aggregation_step_ts = 10;
  separator sep(0, opt);

  t << is_true<expect>( sep.add(0, 1, 1) ) << FAS_FL;
  t << is_true<expect>( sep.add(5, 2, 1) ) << FAS_FL;
  sep.separate(20, nullptr, false);
  t << is_false<expect>( sep.add(5, 3, 1) ) << FAS_FL;

  sep.separate(20, nullptr, true);
  auto s = sep.pop();
  t << is_true<assert>( s != nullptr ) << FAS_FL;
  t << stop;
  t << equal<expect, size_t>( s->count, 2ul ) << FAS_FL;
  t << nothing;
}
}

BEGIN_SUITE(separator, "")
  ADD_UNIT(separator0)
  ADD_UNIT(separator1)
  ADD_UNIT(separator2)
  ADD_UNIT(separator_stale_ts)
END_SUITE(separator)


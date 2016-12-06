#include <fas/testing.hpp>
#include <wrtstat/reducer.hpp>

namespace {

UNIT(reducer, "")
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.limit = 8;
  ro.levels = 4;
  reducer r(ro);
  int value = 0;
  int avg = 0;

  for (int i3=0; i3 < 2; ++i3)
  {
    for (int i2=0; i2 < 4; ++i2)
    {
      for (int i1=0; i1 < 8; ++i1)
      {
        r.add( ++value, 2 );
        avg = (avg + value)/2;
      }
    }
  }
  // Заполнили все доступные массивы в первой итерации i3
  t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect>( r.size(), 8*4 ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect>( r.total_count(), (8*4*2)*2 ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect>( r.lossy_count(), r.total_count() - ro.limit ) << FAS_FL;
  // Минимальное значение
  t << equal<expect>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect>( r.max(), r.total_count()/2 ) << FAS_FL;

  auto res = r.detach();

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, 8*4*2 ) << FAS_FL;
  t << equal<expect, int>( res->count, (8*4*2)*2 ) << FAS_FL;
  t << equal<expect, int>( res->lossy, (8*4*2)*2 - ro.limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.limit ) << FAS_FL;
  for ( auto i : res->data)
    t << message("") << i;
}

}

BEGIN_SUITE(basic, "")
  ADD_UNIT(reducer)
END_SUITE(basic)


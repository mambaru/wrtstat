#include <fas/testing.hpp>
#include <wrtstat/reducer.hpp>

namespace {

template< int Levels, int Limit, typename T>
void empty_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.limit = Limit;
  ro.levels = Levels;
  reducer r(ro);
  r.add(1,1);
  // Заполнили все доступные массивы в первой итерации i3
  t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect>( r.size(), 0 ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect>( r.total_count(), 1 ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect>( r.lossy_count(), 1 ) << FAS_FL;
  // Минимальное значение
  t << equal<expect>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect>( r.max(), 1 ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res==nullptr ) << FAS_FL;
  t << stop;
}
  
UNIT(reducer_0_0, "")
{
  empty_reducer<0,0>(t);
}

UNIT(reducer_0_1, "")
{
  empty_reducer<0,1>(t);
}

UNIT(reducer_1_0, "")
{
  empty_reducer<1,0>(t);
}

template< int Levels, int Limit, typename T>
void filled_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.limit = Limit;
  ro.levels = Levels;
  reducer r(ro);
  int value = 0;
  int avg = 0;

  for (int i3=0; i3 < 2; ++i3)
  {
    for (int i2=0; i2 < Levels; ++i2)
    {
      for (int i1=0; i1 < Limit; ++i1)
      {
        r.add( ++value, 2 );
        avg = (avg + value)/2;
      }
    }
  }
  // Заполнили все доступные массивы в первой итерации i3
  t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect>( r.size(), ro.limit*ro.levels ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect>( r.total_count(), value*2 ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect>( r.lossy_count(), r.total_count() - ro.limit ) << FAS_FL;
  // Минимальное значение
  t << equal<expect>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect>( r.max(), value ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, value*2 ) << FAS_FL;
  t << equal<expect, int>( res->lossy, value*2 - ro.limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.limit ) << FAS_FL;
  for ( auto i : res->data)
    t << message("A: ") << i;
  r.clear();
  t << is_false<expect>( r.filled() ) << FAS_FL;
  t << equal<expect>( r.size(), 0 ) << FAS_FL;
  t << equal<expect>( r.total_count(), 0 ) << FAS_FL;
  t << equal<expect>( r.lossy_count(), 0 ) << FAS_FL;
  t << equal<expect>( r.min(), 0 ) << FAS_FL;
  t << equal<expect>( r.max(), 0 ) << FAS_FL;
  res = r.detach();
  t << is_true<assert>( res==nullptr ) << FAS_FL;
  t << stop;
  
  // count == 0
  value = 0;
  avg = 0;

  for (int i3=0; i3 < 2; ++i3)
  {
    for (int i2=0; i2 < Levels; ++i2)
    {
      for (int i1=0; i1 < Limit; ++i1)
      {
        r.add( ++value, 0 );
        avg = (avg + value)/2;
      }
    }
  }

  t << is_true<expect>( r.filled() ) << FAS_FL;
  t << equal<expect>( r.size(), ro.limit*ro.levels ) << FAS_FL;
  t << equal<expect>( r.total_count(), 0 ) << FAS_FL;
  t << equal<expect>( r.lossy_count(), value - ro.limit ) << FAS_FL;
  t << equal<expect>( r.min(), 1 ) << FAS_FL;
  t << equal<expect>( r.max(), value  ) << FAS_FL;

  res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, 0 ) << FAS_FL;
  t << equal<expect, int>( res->lossy, value - ro.limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.limit ) << FAS_FL;
  for ( auto i : res->data)
    t << message("B: ") << i;
}

UNIT(reducer_1_1, "")
{
  filled_reducer<1,1>(t);
}

UNIT(reducer_1_2, "")
{
  filled_reducer<1,2>(t);
}

UNIT(reducer_2_1, "")
{
  filled_reducer<2,1>(t);
}

UNIT(reducer_4_8, "")
{
  filled_reducer<4,8>(t);
}

UNIT(reducer_8_4, "")
{
  filled_reducer<8,4>(t);
}

template< int Levels, int Limit, int Tail, typename T>
void non_filled_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.limit = Limit;
  ro.levels = Levels;
  reducer r(ro);
  int value = 0;
  int avg = 0;

  for (int i2=0; i2 < Levels - 1; ++i2)
  {
    for (int i1=0; i1 < Limit; ++i1)
    {
      r.add( ++value, 1 );
      avg = (avg + value)/2;
    }
  }

  for (int i3=0; i3 < Tail; ++i3)
  {
    r.add( ++value, 1 );
    avg = (avg + value)/2;
  }
  
  // Заполнили все доступные массивы в первой итерации i3
  t << is_false<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect>( r.size(), ro.limit*(ro.levels-1) + Tail ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect>( r.total_count(), value ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect>( r.lossy_count(), r.total_count() - ro.limit ) << FAS_FL;
  // Минимальное значение
  t << equal<expect>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect>( r.max(), value ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, value ) << FAS_FL;
  t << equal<expect, int>( res->lossy, value - ro.limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.limit ) << FAS_FL;
}

UNIT(reducer_4_8_2, "")
{
  non_filled_reducer<4,8,2>(t);
}

UNIT(reducer_43_29_13, "")
{
  non_filled_reducer<43,29,13>(t);
}

}

BEGIN_SUITE(reducer, "")
  ADD_UNIT(reducer_0_0)
  ADD_UNIT(reducer_0_1)
  ADD_UNIT(reducer_1_0)
  ADD_UNIT(reducer_1_1)
  ADD_UNIT(reducer_1_2)
  ADD_UNIT(reducer_2_1)
  ADD_UNIT(reducer_4_8)
  ADD_UNIT(reducer_8_4)
  ADD_UNIT(reducer_4_8_2)
  ADD_UNIT(reducer_43_29_13)
END_SUITE(reducer)


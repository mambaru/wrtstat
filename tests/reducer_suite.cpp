#include <fas/testing.hpp>
#include <wrtstat/reducer.hpp>
#include <numeric>


namespace {

template< int Limit, int Levels,  typename T>
void empty_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.reducer_limit = Limit;
  ro.reducer_levels = Levels;
  reducer r(ro);
  r.add(1,1);
  // Заполнили все доступные массивы в первой итерации i3
  t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect, size_t>( r.size(), 0 ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect, size_t>( r.total_count(), 1 ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect, size_t>( r.lossy_count(), 1 ) << FAS_FL;
  // Минимальное значение
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect, size_t>( r.max(), 1 ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;
  t << equal<expect, size_t>( res->count, 1 ) << FAS_FL; 
  t << equal<expect, size_t>( res->lossy, 1 ) << FAS_FL;
  t << equal<expect, size_t>( res->lossy, 1 ) << FAS_FL;
  t << equal<expect, size_t>( res->min, 1) << FAS_FL;
  t << equal<expect, size_t>( res->max, 1) << FAS_FL;
  t << equal<expect, size_t>( res->avg, 1) << FAS_FL;
}
  
UNIT(empty_reducer_0_0, "")
{
  empty_reducer<0,0>(t);
}

UNIT(empty_reducer_0_1, "")
{
  empty_reducer<0,1>(t);
}

UNIT(empty_reducer_1_0, "")
{
  empty_reducer<1,0>(t);
}

template< int Limit, int Levels,  typename T>
void filled_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.reducer_limit = Limit;
  ro.reducer_levels = Levels;
  reducer r(ro);
  t << is_false<expect>( r.filled() ) << FAS_FL;
  int value = 0;
  std::vector<int> values;

  for (int i3=0; i3 < 2; ++i3)
  {
    for (int i2=0; i2 < Levels; ++i2)
    {
      for (int i1=0; i1 < Limit; ++i1)
      {
        r.add( ++value, 2 );
        values.push_back(value);
        //avg = (avg + value)/2;
      }
    }
  }
  
  int avg = std::accumulate(values.begin(), values.end(), 0 ) / static_cast<int>( values.size() );
  
  // Заполнили все доступные массивы в первой итерации i3
  t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect, size_t>( r.size(), ro.reducer_limit*ro.reducer_levels ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect, size_t>( r.total_count(), value*2 ) << FAS_FL;
  t << message("values.size=") << values.size() << " r.total_count()=" << r.total_count();
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect, size_t>( r.lossy_count(), r.total_count() - ro.reducer_limit ) << FAS_FL;
  // Минимальное значение
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect, size_t>( r.max(), value ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, long>( res->min, 1 ) << FAS_FL;
  t << equal<expect, long>( res->max, value ) << FAS_FL;
  t << equal<expect, long>( res->count, value*2 ) << FAS_FL;
  t << equal<expect, long>( res->lossy, size_t(value)*2 - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, long>( res->avg, avg ) << FAS_FL;
  t << equal<expect, long>( res->data.size(), ro.reducer_limit ) << FAS_FL;
  /*for ( auto i : res->data)
    t << message("A: ") << i;*/
  r.clear();
  t << is_false<expect>( r.filled() ) << FAS_FL;
  t << equal<expect, size_t>( r.size(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.total_count(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.lossy_count(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.min(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.max(), 0 ) << FAS_FL;
  res = r.detach();
  t << is_true<assert>( res==nullptr ) << FAS_FL;
  t << stop;
  
  // count == 0
  value = 0;
  values.clear();
  //avg = 0;

  for (int i3=0; i3 < 2; ++i3)
  {
    for (int i2=0; i2 < Levels; ++i2)
    {
      for (int i1=0; i1 < Limit; ++i1)
      {
        r.add( ++value, 0 );
        //avg = (avg + value)/2;
        values.push_back(value);
      }
    }
  }

  avg = std::accumulate(values.begin(), values.end(), 0 ) / static_cast<int>( values.size() );
  
  t << is_true<expect>( r.filled() ) << FAS_FL;
  t << equal<expect, size_t>( r.size(), ro.reducer_limit*ro.reducer_levels ) << FAS_FL;
  t << equal<expect, size_t>( r.total_count(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.lossy_count(), size_t(value) - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  t << equal<expect, size_t>( r.max(), value  ) << FAS_FL;

  res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, 0 ) << FAS_FL;
  t << equal<expect, int>( res->lossy, size_t(value) - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.reducer_limit ) << FAS_FL;
  /*for ( auto i : res->data)
    t << message("B: ") << i;*/
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

template< int Limit, int Levels,  typename T>
void filled_reducer_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.reducer_limit = Limit;
  ro.reducer_levels = Levels;
  reducer r0(ro);
  reducer r(ro);
  t << is_false<expect>( r.filled() ) << FAS_FL;
  int value = 0;
  std::vector<int> values;
  std::vector<int> values_avg;

  for (int i3=0; i3 < Levels; ++i3)
  {
    for (int i2=0; i2 < Levels; ++i2)
    {
      for (int i1=0; i1 < Limit; ++i1)
      {
        r0.add( ++value, 2 );
        values.push_back(value);
        //avg = (avg + value)/2;
      }
    };
    if ( auto p = r0.detach() )
    {
      /*for ( int i = 0 ; i < Levels; ++i)
        values_avg.push_back( static_cast<int>(p->avg));
        */
      std::copy(p->data.begin(), p->data.end(), std::back_inserter(values_avg));
      
      t << message("current lossy=") << p->lossy << " count=" << p->count << " size=" << p->data.size() << " avg=" << p->avg;
      r.add(*p);
    }
    else
      t << error("empty detach") << FAS_FL;
    r0.clear();
  }
  
  int avg_real = std::accumulate(values.begin(), values.end(), 0 ) / static_cast<int>( values.size() );
  int avg = std::accumulate(values_avg.begin(), values_avg.end(), 0 ) / static_cast<int>( values_avg.size() );
  t << message("avg=") << avg << " true avg=" << avg_real;
  // Заполнили все доступные массивы в первой итерации i3
  t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect, size_t>( r.size(), ro.reducer_limit*ro.reducer_levels ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect, size_t>( r.total_count(), value*2 ) << FAS_FL;
  t << message("values.size=") << values.size() << " r.total_count()=" << r.total_count();
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect, size_t>( r.lossy_count(), r.total_count() - ro.reducer_limit ) << FAS_FL;
  // Минимальное значение
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect, size_t>( r.max(), value ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, long>( res->min, 1 ) << FAS_FL;
  t << equal<expect, long>( res->max, value ) << FAS_FL;
  t << equal<expect, long>( res->count, value*2 ) << FAS_FL;
  t << equal<expect, long>( res->lossy, size_t(value)*2 - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, long>( res->avg, avg ) << FAS_FL;
  t << equal<expect, long>( res->data.size(), ro.reducer_limit ) << FAS_FL;
  /*for ( auto i : res->data)
    t << message("A: ") << i;*/
  r.clear();
  t << is_false<expect>( r.filled() ) << FAS_FL;
  t << equal<expect, size_t>( r.size(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.total_count(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.lossy_count(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.min(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.max(), 0 ) << FAS_FL;
  res = r.detach();
  t << is_true<assert>( res==nullptr ) << FAS_FL;
  t << stop;
  
  // count == 0
  value = 0;
  values.clear();
  //avg = 0;

  for (int i3=0; i3 < 2; ++i3)
  {
    for (int i2=0; i2 < Levels; ++i2)
    {
      for (int i1=0; i1 < Limit; ++i1)
      {
        r.add( ++value, 0 );
        //avg = (avg + value)/2;
        values.push_back(value);
      }
    }
  }

  avg = std::accumulate(values.begin(), values.end(), 0 ) / static_cast<int>( values.size() );
  
  t << is_true<expect>( r.filled() ) << FAS_FL;
  t << equal<expect, size_t>( r.size(), ro.reducer_limit*ro.reducer_levels ) << FAS_FL;
  t << equal<expect, size_t>( r.total_count(), 0 ) << FAS_FL;
  t << equal<expect, size_t>( r.lossy_count(), size_t(value) - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  t << equal<expect, size_t>( r.max(), value  ) << FAS_FL;

  res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, 0 ) << FAS_FL;
  t << equal<expect, int>( res->lossy, size_t(value) - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.reducer_limit ) << FAS_FL;
  /*for ( auto i : res->data)
    t << message("B: ") << i;*/
}

UNIT(reducer_reducer_1_1, "")
{
  filled_reducer_reducer<1,1>(t);
}

UNIT(reducer_reducer_1_2, "")
{
  filled_reducer_reducer<1,2>(t);
}

UNIT(reducer_reducer_2_1, "")
{
  filled_reducer_reducer<2,1>(t);
}

UNIT(reducer_reducer_4_8, "")
{
  filled_reducer_reducer<4,8>(t);
}

UNIT(reducer_reducer_8_4, "")
{
  filled_reducer<8,4>(t);
}

template< int Limit, int Levels,  int Tail, typename T>
void non_filled_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.reducer_limit = Limit;
  ro.reducer_levels = Levels;
  reducer r(ro);
  int value = 0;
  std::vector<int> values;

  for (int i2=0; i2 < Levels - 1; ++i2)
  {
    for (int i1=0; i1 < Limit; ++i1)
    {
      r.add( ++value, 1 );
      values.push_back(value);
    }
  }

  for (int i3=0; i3 < Tail; ++i3)
  {
    r.add( ++value, 1 );
    values.push_back(value);
  }
  
  int avg = std::accumulate(values.begin(), values.end(), 0 ) / static_cast<int>( values.size() );
  
  // Заполнили все доступные массивы в первой итерации i3
  t << is_false<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  t << equal<expect, size_t>( r.size(), ro.reducer_limit*(ro.reducer_levels-1) + Tail ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect, size_t>( r.total_count(), value ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  t << equal<expect, size_t>( r.lossy_count(), r.total_count() - ro.reducer_limit ) << FAS_FL;
  // Минимальное значение
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect, size_t>( r.max(), value ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, value ) << FAS_FL;
  t << equal<expect, int>( res->lossy, size_t(value) - ro.reducer_limit ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  t << equal<expect, int>( res->data.size(), ro.reducer_limit ) << FAS_FL;
}

UNIT(nf_reducer_4_8_2, "")
{
  non_filled_reducer<4,8,2>(t);
}

UNIT(nf_reducer_43_29_13, "")
{
  non_filled_reducer<43,29,13>(t);
}

UNIT(nf_reducer_29_43_1, "")
{
  non_filled_reducer<29,43,1>(t);
}

UNIT(nf_reducer_1600_2_1500, "")
{
  non_filled_reducer<1600,2,1500>(t);
}

template< int Limit, int Levels,  int Size, typename T>
void array_reducer(T& t)
{
  using namespace fas::testing;
  using namespace wrtstat;
  reducer_options ro;
  ro.reducer_limit = Limit;
  ro.reducer_levels = Levels;
  reducer r(ro);
  int value = 0;
  std::vector<int> values;

  data_type data;
  for (int i=0; i < Size;  ++i)
  {
    data.push_back( ++value );
    values.push_back(value);
  }
  r.add( data );
  int avg = std::accumulate(values.begin(), values.end(), 0 ) / static_cast<int>( values.size() );
  // Заполнили все доступные массивы в первой итерации i3
  if ( Size < Limit*Levels )
    t << is_false<expect>( r.filled() ) << FAS_FL;
  else
    t << is_true<expect>( r.filled() ) << FAS_FL;
  // Суммарный размер массивов при полном заполнении = limit*levels
  int size = Levels*Limit <  Size ? Levels*Limit : Size ;
  t << equal<expect, size_t>( r.size(), size ) << FAS_FL;
  // Суммарный вызов add * 2 (передавали count == 2 )
  t << equal<expect, size_t>( r.total_count(), value ) << FAS_FL;
  // Расчетное количество выброшенных значений при прореживании (останется только массив длиной ro.limit)
  int lossy = Limit < Size ? Size - Limit : 0;
  t << equal<expect, size_t>( r.lossy_count(), lossy ) << "[" << Limit << "," << Levels << "," << Size << "]" << FAS_FL;
  // Минимальное значение
  t << equal<expect, size_t>( r.min(), 1 ) << FAS_FL;
  // Текущее максимальное значение
  t << equal<expect, size_t>( r.max(), value ) << FAS_FL;

  auto res = r.detach();
  t << is_true<assert>( res!=nullptr ) << FAS_FL;
  t << stop;

  t << equal<expect, int>( res->min, 1 ) << FAS_FL;
  t << equal<expect, int>( res->max, value ) << FAS_FL;
  t << equal<expect, int>( res->count, value ) << FAS_FL;
  t << equal<expect, int>( res->lossy, lossy ) << FAS_FL;
  t << equal<expect, int>( res->avg, avg ) << FAS_FL;
  int data_size = Size < Limit ? Size : Limit;
  t << equal<expect, int>( res->data.size(), data_size ) << FAS_FL;
}

UNIT(array_reducer_1_2_3, "")
{
  array_reducer<1,2,3>(t);
}

UNIT(array_reducer_1_2_144, "")
{
  array_reducer<1,2,144>(t);
}

UNIT(array_reducer_43_29_13, "")
{
  array_reducer<43,29,13>(t);
}

UNIT(array_reducer_33_11_1333, "")
{
  array_reducer<33,11,1333>(t);
}

}

BEGIN_SUITE(reducer, "")
  ADD_UNIT(empty_reducer_0_0)
  ADD_UNIT(empty_reducer_0_1)
  ADD_UNIT(empty_reducer_1_0)
  ADD_UNIT(reducer_1_1)
  ADD_UNIT(reducer_1_2)
  ADD_UNIT(reducer_2_1)
  ADD_UNIT(reducer_4_8)
  ADD_UNIT(reducer_8_4)
  ADD_UNIT(reducer_reducer_1_1)
  ADD_UNIT(reducer_reducer_1_2)
  ADD_UNIT(reducer_reducer_2_1)
  ADD_UNIT(reducer_reducer_4_8)
  ADD_UNIT(reducer_reducer_8_4)
  ADD_UNIT(nf_reducer_4_8_2)
  ADD_UNIT(nf_reducer_43_29_13)
  ADD_UNIT(nf_reducer_29_43_1)
  ADD_UNIT(nf_reducer_1600_2_1500)
  ADD_UNIT(array_reducer_1_2_3)
  ADD_UNIT(array_reducer_1_2_144)
  ADD_UNIT(array_reducer_43_29_13)
  ADD_UNIT(array_reducer_33_11_1333)
END_SUITE(reducer)


#include <fas/testing.hpp>
#include <wrtstat/aggregator.hpp>

namespace {

UNIT(basic1, "")
{
  using namespace fas::testing;
  t << nothing;
}

}

BEGIN_SUITE(basic, "")
  ADD_UNIT(basic1)
END_SUITE(basic)


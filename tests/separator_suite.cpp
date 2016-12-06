#include <fas/testing.hpp>
#include <wrtstat/reducer.hpp>

namespace {

UNIT(separator1, "")
{
  using namespace fas::testing;
  using namespace wrtstat;
  t << nothing;
}

}

BEGIN_SUITE(separator, "")
  ADD_UNIT(separator1)
END_SUITE(separator)


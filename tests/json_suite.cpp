#include <wrtstat/api/multi_push_json_compact.hpp>
#include <wrtstat/api/multi_push_json.hpp>
#include <wrtstat/api/push_json.hpp>
#include <wrtstat/api/push_json_compact.hpp>
#include <wrtstat/api/del_json.hpp>

#include <wrtstat/aggregator/api/json/aggregated_perc_json_compact.hpp>
#include <wrtstat/aggregator/api/json/aggregated_perc_json.hpp>
#include <wrtstat/aggregator/api/json/aggregated_info_json_compact.hpp>
#include <wrtstat/aggregator/api/json/aggregated_info_json.hpp>
#include <wrtstat/aggregator/api/json/aggregated_data_json.hpp>
#include <wrtstat/aggregator/api/json/aggregated_data_json_compact.hpp>

#include <wrtstat/aggregator/api/json/reduced_data_json.hpp>
#include <wrtstat/aggregator/api/json/reduced_info_json.hpp>
#include <wrtstat/aggregator/api/json/reduced_info_json_compact.hpp>

#include <wrtstat/multi_aggregator/multi_aggregator_options_json.hpp>

#include <fas/testing.hpp>
#include <wjson/_json.hpp>
#include <numeric>

namespace {

template<typename J, typename T>
std::ostream&  check_json(T& t, const std::string& expect_json)
{
  using namespace fas::testing;
  std::string json;
  typename J::serializer()( typename J::target(), std::back_inserter(json) );
  return t << equal<expect>(json, expect_json) ;
}

UNIT(json1, "")
{
  using namespace fas::testing;
  using namespace wjson::literals;
  using namespace wrtstat;

  check_json<reduced_info_json>(t, "{'ts':0,'avg':0,'count':0,'lossy':0,'min':0,'max':0}"_json ) << FAS_FL;
  check_json<reduced_info_json_compact>(t, "[0,0,0,0,0,0]"_json ) << FAS_FL;
  check_json<reduced_data_json>(t, "{'ts':0,'avg':0,'count':0,'lossy':0,'min':0,'max':0,'data':[]}"_json ) << FAS_FL;

  check_json<aggregated_perc_json>(t, "{'perc50':0,'perc80':0,'perc95':0,'perc99':0,'perc100':0}"_json ) << FAS_FL;
  check_json<aggregated_perc_json_compact>(t, "[0,0,0,0,0]"_json ) << FAS_FL;

  check_json<aggregated_info_json>(t,
    "{'ts':0,'avg':0,'count':0,'perc50':0,'perc80':0,'perc95':0,'perc99':0,'perc100':0,'lossy':0,'min':0,'max':0}"_json ) << FAS_FL;
  check_json<aggregated_info_json_compact>(t, "[0,0,0,0,0,0,0,0,0,0,0]"_json ) << FAS_FL;

  check_json<aggregated_data_json>(t,
    "{'ts':0,'avg':0,'count':0,'perc50':0,'perc80':0,'perc95':0,'perc99':0,'perc100':0,'lossy':0,'min':0,'max':0,'data':[]}"_json ) << FAS_FL;
  check_json<aggregated_data_json_compact>(t, "[0,0,0,0,0,0,0,0,0,0,0,[]]"_json ) << FAS_FL;
}

UNIT(json2, "")
{
  using namespace fas::testing;
  using namespace wjson::literals;
  using namespace wrtstat;

  check_json<request::push_json>(t,
    "{'name':'','ts':0,'avg':0,'count':0,'perc50':0,'perc80':0,'perc95':0,'perc99':0,'perc100':0,'lossy':0,'min':0,'max':0,'data':[]}"_json ) << FAS_FL;
  check_json<response::push_json>(t, "{'status':true}"_json ) << FAS_FL;

  check_json<request::del_json>(t, "{'names':[]}"_json ) << FAS_FL;
  check_json<response::del_json>(t, "{}"_json ) << FAS_FL;

  check_json<request::multi_push_json>(t, "{'sep':'~~','legend':[],'data':[]}"_json ) << FAS_FL;
  check_json<response::multi_push_json>(t, "{'status':true,'error':''}"_json ) << FAS_FL;

  check_json<request::push_json_compact>(t, "['',0,0,0,0,0,0,0,0,0,0,0,[]]"_json ) << FAS_FL;
  check_json<response::push_json_compact>(t, "[true]"_json ) << FAS_FL;

  check_json<request::multi_push_json_compact>(t, "['~~',[],[]]"_json ) << FAS_FL;
  check_json<response::multi_push_json_compact>(t, "[true,'']"_json ) << FAS_FL;

}

} // namespace

BEGIN_SUITE(json, "")
  ADD_UNIT(json1)
  ADD_UNIT(json2)
END_SUITE(json)


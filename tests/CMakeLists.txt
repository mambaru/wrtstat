add_executable(
  test_wrtstat
  test_wrtstat.cpp
  reducer_suite.cpp
  separator_suite.cpp
  aggregator_suite.cpp
  agmap_suite.cpp
  wrtstat_suite.cpp
  ${FAS_TESTING_CPP}
)

target_link_libraries(test_wrtstat wrtstat boost_system)
add_test( test_wrtstat test_wrtstat)

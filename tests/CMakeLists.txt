
add_executable(
  test_wrtstat
  test_wrtstat.cpp
  reducer_suite.cpp
  separator_suite.cpp
  aggregator_suite.cpp
  agmap_suite.cpp
  wrtstat_suite.cpp
)

target_link_libraries(test_wrtstat PUBLIC wrtstat fas::testing)

wci_test( TARGET test_wrtstat COMMAND test_wrtstat)


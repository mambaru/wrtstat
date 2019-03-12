set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})
  
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

target_link_libraries(test_wrtstat wrtstat)

add_test( NAME test_wrtstat COMMAND test_wrtstat)

if ( NOT CODE_COVERAGE )
  add_test(
    NAME test_wrtstat-memcheck 
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/memcheck.sh ${CMAKE_CURRENT_BINARY_DIR}/test_wrtstat
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
  )
endif()


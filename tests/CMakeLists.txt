
if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror -pedantic -fno-rtti -std=c++11")
  message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
endif(UNIX)

if ( NOT FAS_TESTING_CPP )
  set( FAS_TESTING_CPP ${CMAKE_SOURCE_DIR}/../faslib/fas/testing/testing.cpp )
  include_directories(${CMAKE_SOURCE_DIR})
  include_directories(${CMAKE_SOURCE_DIR}/../faslib)
endif()

enable_testing()

include_directories(..)

add_executable(
  test_wrtstat
  test_wrtstat.cpp
  reducer_suite.cpp
  separator_suite.cpp
  aggregator_suite.cpp
  ${FAS_TESTING_CPP}
)

add_test( test_wrtstat test_wrtstat)

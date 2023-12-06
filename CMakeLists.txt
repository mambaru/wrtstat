cmake_minimum_required(VERSION 3.10)
project(wrtstat VERSION "0.9.8")

include(cmake/ci.cmake)

wci_submodule(NAME faslib)
wci_submodule(NAME wjson)

add_subdirectory(wrtstat)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
    add_subdirectory(benchmarks)
  endif()
endif()

include(cmake/install.cmake)

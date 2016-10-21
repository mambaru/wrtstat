cmake_minimum_required(VERSION 2.6)

project(wrtstat)


if ( WRTSTAT_BUILD_TEST OR WRTSTAT_BUILD_ALL )
  add_subdirectory(tests)
  if( WRTSTAT_BUILD_ALL )
    add_subdirectory(examples)
  endif() # WRTSTAT_BUILD_TEST
endif()


if ( NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
  message(STATUS "${PROJECT_NAME} is not top level project")
  return()
endif()

include(mambaopt)
include(mambalibs)


if (NOT WRTSTAT_DISABLE_JSON OR BUID_TESTING)
  get_faslib()
endif()

if ( WRTSTAT_DISABLE_JSON )
  add_definitions(-DWRTSTAT_DISABLE_JSON)
else()
  get_mambaru(wjson WJSON_DIR "")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})



cmake_minimum_required(VERSION 3.1)
project(wrtstat)

include(cmake/ci.cmake)

wci_getlib(NAME faslib)
wci_getlib(NAME wjson)
wci_getlib(NAME wlog)

include_directories(.)
set(
  source_list
  wrtstat/reducer.cpp
  wrtstat/separator.cpp
  wrtstat/aggregator.cpp
  wrtstat/manager/aggregator_map.cpp
  wrtstat/manager/aggregator_hashmap.cpp
  wrtstat/manager/aggregator_registry.cpp
  wrtstat/meter_manager.cpp
  wrtstat/json/load.cpp
)

add_library(wrtstat ${source_list})
wci_targets(wrtstat)
target_include_directories(wrtstat PUBLIC ${wrtstat_SOURCE_DIR})
include(FindThreads)
target_link_libraries(wrtstat PUBLIC faslib wjson wlog ${CMAKE_THREAD_LIBS_INIT})

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
    add_subdirectory(benchmarks)
  endif()
endif()

set(CPACK_PACKAGE_NAME "wrtstat")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_PACKAGE_CONTACT "migashko@gmail.com")

install(DIRECTORY wrtstat DESTINATION include FILES_MATCHING PATTERN "*.hpp" )
install(TARGETS wrtstat LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
include(CPack)



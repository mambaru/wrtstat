cmake_minimum_required(VERSION 3.1)
project(wrtstat)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

rootlibs(faslib wjson wlog)

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

if ( BUILD_TESTING )
  enable_testing()
  target_link_libraries(wrtstat wlog)
  add_subdirectory(tests)
  add_subdirectory(examples)
  add_subdirectory(benchmarks)
endif()

set(CPACK_PACKAGE_NAME "wrtstat")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "8")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_PACKAGE_CONTACT "migashko@gmail.com")

install(DIRECTORY wrtstat DESTINATION include FILES_MATCHING PATTERN "*.hpp" )
install(TARGETS wrtstat LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
include(CPack)



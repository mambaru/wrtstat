cmake_minimum_required(VERSION 3.1)
project(wrtstat)

include(cmake/ci.cmake)

wci_getlib(NAME faslib)
wci_getlib(NAME wjson)

add_subdirectory(wrtstat)

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
set(CPACK_PACKAGE_VERSION_PATCH "3")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_PACKAGE_CONTACT "migashko@gmail.com")

include(CPack)



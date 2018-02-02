cmake_minimum_required(VERSION 2.6)

project(wrtstat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2 ") 
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2 ") 

if ( BUILD_TESTING)
  if( PARANOID_WARNING )
    if(UNIX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align \
                         -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization\
                         -Wformat=2 -Winit-self -Wmissing-declarations \
                         -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual \
                         -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
                         -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror")
      # -Wlogical-op -Wnoexcept -Wstrict-null-sentinel
    endif()
  endif()

  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

include_directories(.)

add_library(
  wrtstat
  wrtstat/reducer.cpp
  wrtstat/separator.cpp
  wrtstat/aggregator.cpp
  wrtstat/manager/aggregator_map.cpp
  wrtstat/manager/aggregator_hashmap.cpp
)



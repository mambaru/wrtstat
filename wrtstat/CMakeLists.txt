add_library(wrtstat
  aggregator/reducer.cpp
  aggregator/separator.cpp
  aggregator/basic_aggregator.cpp
  aggregator/aggregator.cpp
  aggregator/aggregator_mt.cpp
  aggregator/aggregator_registry.cpp
  multi_aggregator/basic_multi_aggregator.cpp
  multi_aggregator/multi_aggregator.cpp
  multi_packer/basic_packer.cpp
  #multi_packer/push_collector.cpp
  options/load_options.cpp
  wrtstat.cpp
)
wci_targets(wrtstat)
target_include_directories(wrtstat PUBLIC $<BUILD_INTERFACE:${wrtstat_SOURCE_DIR}>)
target_link_libraries(wrtstat PUBLIC faslib wjson)

install(DIRECTORY wrtstat DESTINATION include/wrtstat FILES_MATCHING PATTERN "*.hpp" )
install(TARGETS wrtstat LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

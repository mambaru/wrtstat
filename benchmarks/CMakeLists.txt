add_executable(bm_reduce bm_reduce.cpp)
target_link_libraries(bm_reduce wrtstat)

add_executable(bm_aggregator bm_aggregator.cpp)
target_link_libraries(bm_aggregator wrtstat)

wci_utils(bm_reduce bm_aggregator)

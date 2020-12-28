add_executable(bm_reduce bm_reduce.cpp)
target_link_libraries(bm_reduce PRIVATE wrtstat)

add_executable(bm_aggregator bm_aggregator.cpp)
target_link_libraries(bm_aggregator PRIVATE wrtstat)

wci_benchmarks(bm_reduce bm_aggregator)

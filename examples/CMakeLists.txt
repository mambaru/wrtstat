add_executable(example1 example1.cpp)
add_executable(example2 example2.cpp)
add_executable(example3 example3.cpp)
add_executable(example4 example4.cpp)
add_executable(example5 example5.cpp)
add_executable(example6 example6.cpp)
add_executable(example7 example7.cpp)
add_executable(example8 example8.cpp)
add_executable(example9 example9.cpp)
add_executable(example_j example_j.cpp)

include(FindThreads) 

target_link_libraries(example1 PUBLIC wrtstat)
target_link_libraries(example2 PUBLIC wrtstat)
target_link_libraries(example3 PUBLIC wrtstat)
target_link_libraries(example4 PUBLIC wrtstat)
target_link_libraries(example5 PUBLIC wrtstat)
target_link_libraries(example6 PUBLIC wrtstat ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(example7 PUBLIC wrtstat)
target_link_libraries(example8 PUBLIC wrtstat ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(example9 PUBLIC wrtstat)
target_link_libraries(example_j PUBLIC wrtstat)

wci_examples(example1 example2 example3 example4 example5
             example6 example7 example8 example9 example_j)



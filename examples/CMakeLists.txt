cmake_minimum_required(VERSION 3.17.2)

add_executable(events events.cxx)
target_link_libraries(events cyan_event)

add_executable(active_object active_object.cxx)
target_link_libraries(active_object cyan_dispatch)

add_executable(active_object2 active_object2.cxx)
target_link_libraries(active_object2 cyan_dispatch)

add_executable(async async.cxx)
target_link_libraries(async cyan_dispatch)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_executable(boost_asio_benchmark boost_asio_benchmark.cxx)
    target_link_libraries(boost_asio_benchmark cyan_dispatch)
endif()

add_executable(factorial_benchmark factorial_benchmark.cxx)
set_target_properties(factorial_benchmark PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -fopenmp-cuda-mode" LINK_FLAGS "${LINK_FLAGS} -fopenmp-cuda-mode")
target_link_libraries(factorial_benchmark cyan_dispatch)

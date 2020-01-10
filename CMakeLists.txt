cmake_minimum_required(VERSION 3.15)
project(horizon_algorithm)

set(CMAKE_CXX_STANDARD 17)

set(HEADER
        include/algorithm/concurrent_base.hpp
        include/algorithm/concurrent_base_lock.hpp
        include/algorithm/concurrent_queue.hpp
        include/algorithm/concurrent_statelist.hpp
        include/algorithm/concurrent_unorderedmap.hpp
        include/algorithm/concurrent_vector.hpp)

set(SOURCE
        )

set(HORIZON_ALGORITHM_COMPILE_SOURCE ${HEADER} ${SOURCE})

add_library(horizon_algorithm INTERFACE)
target_include_directories(horizon_algorithm INTERFACE include/)
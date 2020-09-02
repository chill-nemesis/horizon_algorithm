cmake_minimum_required(VERSION 3.15)
project(horizon_algorithm VERSION 0.0.0 LANGUAGES CXX C)

include(HorizonCommon)

include_directories(include/)

set(HEADER
        include/algorithm/concurrent/concurrent_base.hpp
        include/algorithm/concurrent/concurrent_queue.hpp
        include/algorithm/concurrent/concurrent_statelist.hpp
        include/algorithm/concurrent/concurrent_unorderedmap.hpp
        include/algorithm/concurrent/concurrent_vector.hpp
        include/algorithm/concurrent/ContainerClosedError.hpp
        include/algorithm/stl_extension/vector.hpp
        include/algorithm/stl_extension/inheritable_shared_from_this.hpp
        include/algorithm/UUID.hpp
        )

set(SOURCE
        )

set(HORIZON_ALGORITHM_COMPILE_SOURCE ${HEADER} ${SOURCE})


add_library(horizon_algorithm INTERFACE)
target_link_libraries(horizon_algorithm INTERFACE horizon_preprocessor)
target_include_directories(horizon_algorithm INTERFACE include/)
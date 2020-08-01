cmake_minimum_required(VERSION 3.13)
project(lab_06)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig)
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
		)
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
		)

add_library(function_server function_server.cpp function_server.h)
target_link_libraries(function_server PUBLIC ${ZeroMQ_LIBRARY})
target_include_directories(function_server PUBLIC ${ZeroMQ_INCLUDE_DIR})

add_library(topology topology.cpp topology.h)


add_executable(my_slave my_slave.cpp)
target_include_directories(my_slave PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(my_slave PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(my_slave PUBLIC function_server)
target_link_libraries(my_slave PUBLIC topology)

add_executable(my_server my_server.cpp)
target_include_directories(my_server PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(my_server PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(my_server PUBLIC function_server)
target_link_libraries(my_server PUBLIC topology)

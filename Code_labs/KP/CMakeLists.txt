cmake_minimum_required(VERSION 3.13)
project(kurs_project)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)

add_library(worker worker.cpp worker.h exeption.h) 
add_executable(main_kp main_kp.cpp)
target_link_libraries(main_kp PUBLIC worker)

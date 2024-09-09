cmake_minimum_required(VERSION 3.28)
project(cpbench)

set(CMAKE_CXX_STANDARD 23)

find_package(benchmark REQUIRED)

include_directories(src)
include_directories(third_party)

add_executable(Main src/Main.cpp)
target_link_libraries(Main benchmark::benchmark)
target_include_directories(Main PUBLIC src third_party)
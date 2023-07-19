cmake_minimum_required(VERSION 3.25)
project(cloth)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib CONFIG REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE raylib)


cmake_minimum_required(VERSION 3.14)
project(testshm)

set(CMAKE_CXX_STANDARD 17)

add_executable(testshm main.cpp)
target_compile_options(testshm PRIVATE -Wall -Wextra)
target_link_libraries(testshm rt)
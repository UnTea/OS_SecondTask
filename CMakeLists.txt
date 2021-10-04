cmake_minimum_required(VERSION 3.20)
project(OS_SecondTask)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(sha-256)

add_executable(OS_SecondTask src/main.cpp)
target_link_libraries(OS_SecondTask sha-256)
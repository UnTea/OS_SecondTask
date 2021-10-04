cmake_minimum_required(VERSION 3.20)

add_library(sha-256 STATIC sha-256.cpp)

target_include_directories(sha-256 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
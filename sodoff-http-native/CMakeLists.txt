cmake_minimum_required (VERSION 3.8)
project(sodoff_http C)

set(CMAKE_C_STANDARD 11)

add_library(sodoff_http SHARED library.c)

target_link_libraries(sodoff_http PRIVATE winhttp)

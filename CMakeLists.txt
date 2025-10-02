cmake_minimum_required(VERSION 3.6)
project(yoyo C)

set(CMAKE_C_STANDARD 11)

add_executable(yoyo yoyo.c common.c)
target_link_libraries(yoyo jansson sodium)

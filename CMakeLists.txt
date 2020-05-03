cmake_minimum_required(VERSION 3.0)
project(carraylist C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(carraylist_test array_list.c array_list_test.c)
add_library(carraylist SHARED array_list.c)

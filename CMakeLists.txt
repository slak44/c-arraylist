cmake_minimum_required(VERSION 3.0)
project(carraylist C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

set(CMAKE_VERBOSE_MAKEFILE true)

include(ExternalProject)

ExternalProject_Add(
        criterion
        GIT_REPOSITORY https://github.com/Snaipe/Criterion
        GIT_TAG v2.3.3
        PREFIX ${CMAKE_BINARY_DIR}
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/criterion-prefix/src/criterion/include)
link_directories(${CMAKE_BINARY_DIR}/src/criterion-build)

add_executable(carraylist_test array_list.c tests/array_list_test.c)
target_link_libraries(carraylist_test libcriterion.so)
add_dependencies(carraylist_test criterion)

add_library(carraylist SHARED array_list.c)

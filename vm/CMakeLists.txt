cmake_minimum_required(VERSION 3.13)
project(njvm)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(njvm njvm.c)
target_link_libraries(njvm ${CURSES_LIBRARIES})
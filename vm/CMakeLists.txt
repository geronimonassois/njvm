cmake_minimum_required(VERSION 3.13)
project(njvm)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
include_directories(bigint/build/include)
link_directories(bigint/build/lib)

add_executable(njvm njvm.c debugger/debugger.c debugger/debugger.h)
target_link_libraries(njvm bigint)

set_property(TARGET njvm PROPERTY C_STANDARD 99)

# NCurses Libary for command line program
#target_link_libraries(njvm ${CURSES_LIBRARIES})
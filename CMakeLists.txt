cmake_minimum_required(VERSION 3.16.1)
project(theta_fs C)

set(CMAKE_C_STANDARD 11)

set(CURSES_NEED_WIDE TRUE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(theta_fs main.c utils.c objects_list.c io.c explorers.c constants.h explorers.h io.h objects_list.h utils.h)
target_link_libraries(theta_fs ${CURSES_LIBRARIES})

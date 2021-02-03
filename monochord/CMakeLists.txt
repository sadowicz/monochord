cmake_minimum_required(VERSION 3.13)
project(monochord C)

set(CMAKE_C_STANDARD 11)

add_executable(monochord monochord.c utils.c utils.h parse.c parse.h udp.c udp.h)
target_link_libraries(monochord PRIVATE m PRIVATE rt)
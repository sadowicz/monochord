cmake_minimum_required(VERSION 3.13)
project(info_rejestrator C)

set(CMAKE_C_STANDARD 11)

add_executable(info_rejestrator rejestrator_info.c utils.c utils.h)
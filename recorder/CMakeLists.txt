cmake_minimum_required(VERSION 3.13)
project(recorder C)

set(CMAKE_C_STANDARD 11)

add_executable(recorder rejestrator.c parse.c parse.h utils.c utils.h)
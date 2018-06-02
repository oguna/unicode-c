cmake_minimum_required(VERSION 3.10)
project(unicode_c)

set(CMAKE_C_STANDARD 11)

add_executable(unicode_c main.c unicode.h unicode.c)
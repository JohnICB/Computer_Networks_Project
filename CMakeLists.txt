cmake_minimum_required(VERSION 3.9)
project(untitled C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled server.c gamedata.c gamedata.h)
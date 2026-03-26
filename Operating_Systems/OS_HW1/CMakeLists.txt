cmake_minimum_required(VERSION 3.30)
project(OS_HW1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(OS_HW1
    Commands.cpp
    Commands.h
    environment.cpp
    environment.h
    ordered_map.h
    signals.cpp
    signals.h
    smash.cpp
    utilies.cpp
    utilies.h)

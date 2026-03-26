cmake_minimum_required(VERSION 3.27)
project(data_structure_1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(data_structure_1
        plains25a1.cpp
        plains25a1.h
        wet1util.h
        horse.h
        herd.h
        avl.h
        test0.cpp
        main25a1.cpp
)

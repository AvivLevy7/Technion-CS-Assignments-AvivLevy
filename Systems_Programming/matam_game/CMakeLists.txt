cmake_minimum_required(VERSION 3.27)
project(matam_HW4)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Events)
include_directories(Players)

add_executable(matam_HW4
        Events/Event.h
        Events/Monster.h
        Players/Character.h
        Players/Job.h
        Players/Player.h
        main.cpp
        MatamStory.cpp
        MatamStory.h
        Players/Player.cpp
        Events/MonsterFactory.h
        Events/MonsterFactory.cpp
        Events/Monster.cpp
)
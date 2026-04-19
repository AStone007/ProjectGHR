cmake_minimum_required(VERSION 4.2)
project(CodingProject C)

set(CMAKE_C_STANDARD 11)

add_executable(CodingProject main.c
        io.c
        wf.c
        wf.h
        io.h)

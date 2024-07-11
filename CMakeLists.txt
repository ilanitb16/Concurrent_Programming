cmake_minimum_required(VERSION 3.28)
project(Concurrent_Programming C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Concurrent_Programming
        main.c
        unbounded_queue.c
        bounded_queue.c
)

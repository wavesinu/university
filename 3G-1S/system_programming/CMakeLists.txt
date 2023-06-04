cmake_minimum_required(VERSION 3.25)
project(system_programming C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(message-queue-assginment)
include_directories(record-lock-assginment)

add_executable(system_programming
        message-queue-assginment/client.c
        message-queue-assginment/server.c
        record-lock-assginment/record-lock.c
        mypipe-sample.c
        mypipe.c
        tolower.c
        toupper.c)

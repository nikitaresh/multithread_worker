cmake_minimum_required(VERSION 3.5)

project(multithread_worker)

include_directories(.)

add_executable(multithread_worker multithread_worker.h multithread_worker.cpp main.cpp)

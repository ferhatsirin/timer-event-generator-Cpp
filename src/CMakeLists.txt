cmake_minimum_required(VERSION 3.10)

project(Timer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCES main.cpp MyTimer.cpp ITimer.cpp)

add_executable(Timer ${SOURCES})

target_link_libraries(Timer ${CMAKE_THREAD_LIBS_INIT})


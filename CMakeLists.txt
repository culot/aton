cmake_minimum_required(VERSION 3.5)
project(aton)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cc")
add_executable(aton ${SOURCES})

# ZeroMQ library
find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

target_include_directories(aton PUBLIC ${ZMQ_INCLUDE_DIR})
target_link_libraries(aton PUBLIC ${ZMQ_LIBRARY})

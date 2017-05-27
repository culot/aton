cmake_minimum_required(VERSION 3.5)
project(tests)

set(CMAKE_CXX_STANDARD 11)

add_executable(status status.cc)
# add_executable(test2 test2.cc)

# ZeroMQ library
find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

target_include_directories(status PUBLIC ${ZMQ_INCLUDE_DIR})
target_link_libraries(status PUBLIC ${ZMQ_LIBRARY})

cmake_minimum_required(VERSION 3.5)
project(tests)

set(CMAKE_CXX_STANDARD 11)

# ZeroMQ library
find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

file(GLOB TST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc)
list(REMOVE_ITEM TST_SOURCES "common.cc")

foreach(testsrcfile ${TST_SOURCES})
  string(REPLACE ".cc" "" testname ${testsrcfile})
  add_executable(${testname} ${testsrcfile} common.cc)
  target_include_directories(${testname} PUBLIC ${ZMQ_INCLUDE_DIR})
  target_link_libraries(${testname} PUBLIC ${ZMQ_LIBRARY})
endforeach(testsrcfile ${TST_SOURCES})

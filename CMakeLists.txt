cmake_minimum_required(VERSION 3.5)
project(aton)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cc")
add_executable(aton ${SOURCES})

# glog library
find_path(GLOG_INCLUDE_DIR glog/logging.h)
find_library(GLOG_LIBRARY glog)

# ZeroMQ library
find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

# SQLite library
find_path(SQLITE_INCLUDE_DIR sqlite3.h)
find_library(SQLITE_LIBRARY NAMES sqlite3)

target_include_directories(aton PUBLIC ${SQLITE_INCLUDE_DIR} ${ZMQ_INCLUDE_DIR} ${GLOG_INCLUDE_DIR})
target_link_libraries(aton PUBLIC ${SQLITE_LIBRARY} ${ZMQ_LIBRARY} ${GLOG_LIBRARY})

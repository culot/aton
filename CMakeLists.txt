cmake_minimum_required (VERSION 3.5)
project (aton)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cc")

add_executable (aton ${SOURCES})

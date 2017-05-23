cmake_minimum_required (VERSION 2.8.9)
project (aton)

file(GLOB SOURCES "src/*.cc")

add_executable (aton ${SOURCES})

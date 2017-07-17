cmake_minimum_required(VERSION 3.5)
project(tools)

set(CMAKE_CXX_STANDARD 11)

file(GLOB TOOLS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc)

foreach(toolsrcfile ${TOOLS_SOURCES})
  string(REPLACE ".cc" "" toolname ${toolsrcfile})
  add_executable(${toolname} ${toolsrcfile})
endforeach(toolsrcfile ${TOOLS_SOURCES})

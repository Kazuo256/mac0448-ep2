
cmake_minimum_required (VERSION 2.8)

project (ep2 CXX C)
include_directories (src)

set (EP2_SRC)

add_executable (
  ep2-client
  src/client.cxx
  src/prompt.cxx
  src/command.cxx
  ${EP2_SRC}
)

add_executable (
  ep2-server
  src/server.cxx
  src/command.cxx
  ${EP2_SRC}
)

set_target_properties (
  ep2-client ep2-server
  PROPERTIES COMPILE_FLAGS
  "-ansi -Wall -g -O0"
)



cmake_minimum_required (VERSION 2.8)

project (ep2 CXX C)
include_directories (src)

set (
  EP2_COMMON_SRC
  src/command.cxx
  src/command.h
  src/TCPconnection.cxx
  src/TCPconnection.h
  src/connection.h
  src/commandhandler.h
  src/eventmanager.cxx
  src/eventmanager.h
)

set (
  EP2_SERVER_SRC
  src/server.cxx
  src/serverhandler.cxx
  src/serverhandler.h
)

set (
  EP2_CLIENT_SRC
  src/client.cxx
  src/prompt.cxx
  src/prompt.h
)

add_executable (ep2-client ${EP2_CLIENT_SRC} ${EP2_COMMON_SRC})

add_executable (ep2-server ${EP2_SERVER_SRC} ${EP2_COMMON_SRC})

set_target_properties (
  ep2-client ep2-server
  PROPERTIES COMPILE_FLAGS
  "-D_GNU_SOURCE -ansi -Wall -g -O0"
)


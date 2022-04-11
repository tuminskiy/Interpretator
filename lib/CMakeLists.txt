cmake_minimum_required(VERSION 3.5)

find_package(Boost REQUIRED)

add_library(lib STATIC)

set(SRC
  include/token.hpp src/token.cpp
  include/lexer.hpp src/lexer.cpp
  include/parser.hpp src/parser.cpp
  include/expression.hpp src/expression.cpp
  include/util.hpp src/util.cpp
)

set_target_properties(lib PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON

  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release
)

target_include_directories(lib PUBLIC include)
target_include_directories(lib PRIVATE ${Boost_INCLUDE_DIR})

target_sources(lib PRIVATE ${SRC})

cmake_minimum_required(VERSION 3.5)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(test_runner)

set(SRC
  src/main.cpp
  src/token_value_tests.cpp
  src/lexer_tests.cpp
  src/parser_tests.cpp
  src/expression_tests.cpp
)

set_target_properties(test_runner PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON

  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release
)

target_include_directories(test_runner PRIVATE include ${Boost_INCLUDE_DIR})

target_sources(test_runner PRIVATE ${SRC})

target_link_libraries(test_runner PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} lib)

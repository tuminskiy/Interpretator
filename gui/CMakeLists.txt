cmake_minimum_required(VERSION 3.5)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(SRC
  src/main.cpp
  include/mainwindow.hpp src/mainwindow.cpp
)

set(UI ui/mainwindow.ui)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(gui)
else()
  add_executable(gui)
endif()

set_target_properties(gui PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON

  AUTOMOC ON
  AUTOUIC ON

  AUTOUIC_SEARCH_PATHS ui

  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release
)

qt_wrap_ui(UI_HEADERS ${UI})

target_include_directories(gui PRIVATE include ${CMAKE_BINARY_DIR}/gui)

target_sources(gui PRIVATE ${SRC} ${UI_HEADERS})

target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets lib)

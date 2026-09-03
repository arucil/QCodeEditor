cmake_minimum_required(VERSION 3.6)
project(QCodeEditorExample)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC On)
set(CMAKE_AUTORCC ON)

if(NOT QT_VERSION)
  set(QT_VERSION Qt5)
endif()
find_package(${QT_VERSION} COMPONENTS Widgets Gui REQUIRED)

add_executable(QCodeEditorExample
    resources/demo_resources.qrc
    src/main.cpp
    src/MainWindow.cpp
    src/DiagnosticListItem.cpp
    include/MainWindow.hpp
    include/DiagnosticListItem.hpp
)

target_include_directories(QCodeEditorExample PUBLIC
    include
)

target_link_libraries(QCodeEditorExample
    ${QT_VERSION}::Core
    ${QT_VERSION}::Widgets
    ${QT_VERSION}::Gui
    QCodeEditor
)

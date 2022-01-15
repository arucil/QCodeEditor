cmake_minimum_required(VERSION 3.6)
project(QCodeEditor)

set(LIB_VERSION_MAJOR 0)
set(LIB_VERSION_MINOR 1)
set(LIB_VERSION_TWEAK 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_TWEAK})

set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)

option(BUILD_EXAMPLE "Example building required" Off)
if (${BUILD_EXAMPLE})
    message(STATUS "QCodeEditor example will be built.")
    add_subdirectory(example)
endif()

set(RESOURCES_FILE
    resources/qcodeeditor_resources.qrc
)

set(INCLUDE_FILES
    include/QCodeEditor
    include/QCXXHighlighter
    include/QStyleSyntaxHighlighter
    include/QSyntaxStyle
    include/QGLSLCompleter
    include/QGLSLHighlighter
    include/QJavaHighlighter
    include/QJSHighlighter
    include/QXMLHighlighter
    include/QJSONHighlighter
    include/QLuaCompleter
    include/QLuaHighlighter
    include/QPythonHighlighter
    include/internal/QHighlightRule.hpp
    include/internal/QHighlightBlockRule.hpp
    include/internal/QCodeEditor.hpp
    include/internal/QCXXHighlighter.hpp
    include/internal/QJavaHighlighter.hpp
    include/internal/QJSHighlighter.hpp
    include/internal/QLineNumberArea.hpp
    include/internal/QStyleSyntaxHighlighter.hpp
    include/internal/QSyntaxStyle.hpp
    include/internal/QGLSLCompleter.hpp
    include/internal/QGLSLHighlighter.hpp
    include/internal/QLanguage.hpp
    include/internal/QXMLHighlighter.hpp
    include/internal/QJSONHighlighter.hpp
    include/internal/QLuaCompleter.hpp
    include/internal/QLuaHighlighter.hpp
    include/internal/QPythonCompleter.hpp
    include/internal/QPythonHighlighter.hpp
)

set(SOURCE_FILES
    src/internal/QCodeEditor.cpp
    src/internal/QLineNumberArea.cpp
    src/internal/QCXXHighlighter.cpp
    src/internal/QSyntaxStyle.cpp
    src/internal/QStyleSyntaxHighlighter.cpp
    src/internal/QGLSLCompleter.cpp
    src/internal/QGLSLHighlighter.cpp
    src/internal/QJavaHighlighter.cpp
    src/internal/QJSHighlighter.cpp
    src/internal/QLanguage.cpp
    src/internal/QXMLHighlighter.cpp
    src/internal/QJSONHighlighter.cpp
    src/internal/QLuaCompleter.cpp
    src/internal/QLuaHighlighter.cpp
    src/internal/QPythonCompleter.cpp
    src/internal/QPythonHighlighter.cpp
)

# Create code for QObjects
set(CMAKE_AUTOMOC On)

# Create code from resource files
set(CMAKE_AUTORCC ON)

# Generate compile_commands.json in build/ for analyzers like clang-tidy.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find includes in corresponding build directories
if(NOT QT_VERSION)
  set(QT_VERSION Qt5)
endif()
find_package(${QT_VERSION} REQUIRED COMPONENTS Core Gui Widgets)

if(NOT TYPEOFLIBRARY)
  set(TYPEOFLIBRARY STATIC)
endif()
add_library(QCodeEditor ${TYPEOFLIBRARY}
    ${RESOURCES_FILE}
    ${SOURCE_FILES}
    ${INCLUDE_FILES}
)

if (TYPEOFLIBRARY STREQUAL "SHARED")
	# Set version of the library
	set_target_properties(QCodeEditor PROPERTIES VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_TWEAK})
	# Create symlink "libQCodeEditor.${LIB_VERSION_MAJOR} ->"
	set_target_properties(QCodeEditor PROPERTIES SOVERSION ${LIB_VERSION_MAJOR})
endif()

target_include_directories(QCodeEditor PUBLIC
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
	PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QCodeEditor>
)

install(TARGETS QCodeEditor EXPORT QCodeEditorTarget
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QCodeEditor
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QCodeEditor
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(QCodeEditor
        PRIVATE
        -pedantic
        -Wall
        -Wextra
        -Woverloaded-virtual
        -Winit-self
        -Wunreachable-code
    )
endif(CMAKE_COMPILER_IS_GNUCXX)

target_link_libraries(QCodeEditor
    ${QT_VERSION}::Core
    ${QT_VERSION}::Widgets
    ${QT_VERSION}::Gui
)

# Install files to use "find_package(QCodeEditor )"
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/QCodeEditor)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QCodeEditorConfigVersion.cmake
    VERSION ${LIB_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/QCodeEditorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QCodeEditorConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/QCodeEditorConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/QCodeEditorConfigVersion.cmake
	DESTINATION ${INSTALL_CONFIGDIR}
)

install(EXPORT QCodeEditorTarget
	FILE QCodeEditorTargets.cmake
	NAMESPACE QCodeEditor::
	DESTINATION ${INSTALL_CONFIGDIR}
)

add_library(QCodeEditor::QCodeEditor ALIAS QCodeEditor)

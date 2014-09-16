cmake_minimum_required(VERSION 2.8)
project(cyclone)

# --------------
# Project setup:
# --------------

# Project version:
set (CYCLONE_VERSION_MAJOR 1)
set (CYCLONE_VERSION_MINOR 0)

# Default build type:
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif (NOT CMAKE_BUILD_TYPE)

set (CMAKE_DEBUG_POSTFIX "-dbg" CACHE STRINGS "Adds a postfix for debug-built libraries")

# Configure a header file to pass the CMake settings:
configure_file (
    "${PROJECT_SOURCE_DIR}/CycloneConfig.h.in"
    "${PROJECT_BINARY_DIR}/CycloneConfig.h"
)

# Add the binary dir to the include path, so that the configuration
# header can be found:
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}")

# Enable testing for this project:
enable_testing ()

include_directories ("${PROJECT_SOURCE_DIR}/lib")
# ------------------
# Compiler features:
# ------------------
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
if (UNIX)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -pedantic")
endif ()

# ----------------------
# External dependencies:
# ----------------------
# set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

# --------
# Testing:
# --------
find_package (Boost REQUIRED unit_test_framework)

# ---------------
# Subdirectories:
# ---------------
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)
cmake_minimum_required(VERSION 3.17.2)

# Paths
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Options
set(BUILD_TESTING ON CACHE BOOL "Build testing tree" FORCE)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InitFlags.cmake")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# CMake policies
cmake_policy(SET CMP0022 NEW)
# On MacOS use @rpath/ for target's install name prefix path
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif ()
# Clear VERSION variables when no VERSION is given to project()
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

# Project
project(cyan
    VERSION 0.0.0.1
    DESCRIPTION "Collection of libraries"
    HOMEPAGE_URL "https://github.com/sayan-chaliha/cyan"
    LANGUAGES C CXX)

# Testing
include(Testing)

# Configure files
include(Platform)
configure_file(config.h.in ${PROJECT_NAME}/config.h)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/config.h DESTINATION include/${PROJECT_NAME})

# Libraries
add_subdirectory(common)
add_subdirectory(lockfree)
add_subdirectory(event)
add_subdirectory(dispatch)
add_subdirectory(net)

# Examples
add_subdirectory(examples)

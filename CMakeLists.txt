cmake_minimum_required(VERSION 3.17.2)

# Options
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

set(CMAKE_BUILD_TESTS OFF CACHE BOOL "Build unit tests" FORCE)
if(CMAKE_BUILD_TESTS)
  enable_testing()
endif()

# Paths
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

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

# Compiler flags
if(MSVC)
    set(CYAN_CXX_FLAGS_COMMON
        /Wall
        /WX
        /std:c++latest
    )
    
    set(CYAN_CXX_FLAGS_DEBUGINFO
        /DEBUG:FULL
    )
    
    set(CYAN_CXX_FLAGS_OPTIMIZATION
        /O3
    )

    set(CYAN_CXX_FLAGS_DEBUG
        /DDEBUG
        /D_DEBUG
    )

    set(CYAN_CXX_FLAGS_RELEASE
        /DNDEBUG
    )
else()
    set(CYAN_CXX_FLAGS_COMMON
        -Wall
        -Wextra
        -pedantic
        -Werror
        -std=c++2a
    )
    
    set(CYAN_CXX_FLAGS_DEBUGINFO
        -g3
    )
    
    set(CYAN_CXX_FLAGS_OPTIMIZATION
        -O3
    )

    set(CYAN_CXX_FLAGS_DEBUG
        -DDEBUG
        -D_DEBUG
    )

    set(CYAN_CXX_FLAGS_RELEASE
        -DNDEBUG
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CYAN_CXX_FLAGS
      ${CYAN_CXX_FLAGS_COMMON}
      ${CYAN_CXX_FLAGS_DEBUGINFO}
      ${CYAN_CXX_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
  set(CYAN_CXX_FLAGS
      ${CYAN_CXX_FLAGS_COMMON}
      ${CYAN_CXX_FLAGS_DEBUGINFO}
      ${CYAN_CXX_FLAGS_OPTIMIZATION}
      ${CYAN_CXX_FLAGS_RELEASE})
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
  set(CYAN_CXX_FLAGS
      ${CYAN_CXX_FLAGS_COMMON}
      ${CYAN_CXX_FLAGS_OPTIMIZATION}
      ${CYAN_CXX_FLAGS_RELEASE})
elseif(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
  set(CYAN_CXX_FLAGS
      ${CYAN_CXX_FLAGS_COMMON}
      ${CYAN_CXX_FLAGS_OPTIMIZATION}
      ${CYAN_CXX_FLAGS_RELEASE)
endif()

add_compile_options(${CYAN_CXX_FLAGS})
add_link_options(-fPIC)

# Configure files
include(Platform)
configure_file(config.h.in ${PROJECT_NAME}/config.h)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/config.h DESTINATION include/${PROJECT_NAME})

# External Projects
include(ExternalProjects)

# Libraries
add_subdirectory(common)
add_subdirectory(lockfree)
add_subdirectory(event)
add_subdirectory(dispatch)

# Examples
add_subdirectory(examples)

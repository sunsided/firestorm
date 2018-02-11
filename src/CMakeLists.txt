cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(libfirestorm VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

# Set default compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g")

# Set module paths for extra functionality.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Check for Boost
find_package(Boost 1.61 REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_property(TARGET boost PROPERTY INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})

# Google Performance Tools
find_package(Gperftools 2.6)
if(GPERFTOOLS_FOUND)
    add_library(gperftools INTERFACE IMPORTED)
    set_property(TARGET gperftools PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GPERFTOOLS_INCLUDE_DIR})
    set_property(TARGET gperftools PROPERTY INTERFACE_LINK_LIBRARIES ${GPERFTOOLS_LIBRARIES})
    add_definitions(-DWITH_GPERFTOOLS)
    # TODO: Add option() to selectively enable tcmalloc and profiler
endif()

# Options
include(OptionSimd)
include(OptionFastmath)
include(OptionOpenMP)

# CMake Features
include(CheckIPOSupported)

# Targets that we develop here
enable_testing()
add_subdirectory(libfirestorm)
add_subdirectory(firestorm)

# Print a summary of the selected options.
include(FeatureSummary)
feature_summary(WHAT ALL)
cmake_minimum_required(VERSION 3.6)
project(firestorm)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(OptimizeForArchitecture)
OptimizeForArchitecture()

if (USE_AVX2)
    message(STATUS "Compiling with AVX2 support")
    set(__AVX2__ 1)
elseif (USE_AVX)
    message(STATUS "Compiling with AVX support")
    set(__AVX__ 1)
else()
    message(FATAL_ERROR "The target machine does not support AVX. At least AVX is required")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_ARCHITECTURE_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCE_FILES main.cpp include/Simd.h)
add_executable(firestorm ${SOURCE_FILES})
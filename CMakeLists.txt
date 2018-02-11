cmake_minimum_required(VERSION 3.6)
project(firestorm)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/eigen/cmake/;${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCE_FILES
        main.cpp
        include/Simd.h
        include/mem_chunk_t.h
        include/ChunkManager.h
        include/bytes_t.h
        include/context_t.h
        include/index_t.h
        include/vector_insert_t.h
        include/Worker.h
        include/chunk_idx_t.h
        include/ChunkVisitor.h
        include/ChunkAccessor.h
        include/DotProductVisitorAvx.h
        include/DotProductVisitorNaiveUnrolled.h
        include/vector_t.h
        include/result_t.h
        include/dot_product_avx256.h
        include/dot_product_naive.h
        dot_product_avx256.cpp
        dot_product_naive.cpp)
add_executable(firestorm ${SOURCE_FILES})

# Require ComputeCPP
set(COMPUTECPP_PACKAGE_ROOT_DIR /opt/computecpp CACHE PATH "Path to ComputeCpp")
include(FindComputeCpp)

# Check for Eigen

# find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)
# target_link_libraries (firestorm PUBLIC Eigen3::Eigen)

set(EIGEN_TEST_SYCL TRUE)
set(EIGEN_TEST_CXX11 TRUE)

set(EIGEN_BUILD_BTL FALSE)
set(EIGEN_COVERAGE_TESTING FALSE)
set(EIGEN_BUILD_PKGCONFIG FALSE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/eigen/")
include_directories("${INCLUDE_DIRECTORIES};${CMAKE_CURRENT_SOURCE_DIR}/external/eigen;${CMAKE_CURRENT_SOURCE_DIR}/external/eigen/unsupported")

# Check for AVX and AVX2
# TODO: This is local machine-centric. Make this cross-compilable easily.

# Local machine optimization
include(OptimizeForArchitecture)
OptimizeForArchitecture()

if (USE_AVX2)
    message(STATUS "Compiling with AVX2 support")
    set(__AVX2__ 1)
    add_definitions(-DAVX_VERSION=2)
elseif (USE_AVX)
    message(STATUS "Compiling with AVX support")
    set(__AVX__ 1)
    add_definitions(-DAVX_VERSION=1)
else()
    message(WARNING "The current machine does not support AVX. At least AVX is required for optimal performance.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_ARCHITECTURE_FLAGS}")

# Compiler optimizations

if(CMAKE_COMPILER_IS_GNUCXX)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Check for Boost
find_package(Boost 1.61 REQUIRED)

if(Boost_FOUND)
    message(STATUS "Found Boost ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(firestorm ${Boost_LIBRARIES})
endif()

# Google Performance Tools
find_package(Gperftools 2.6)
if(GPERFTOOLS_FOUND)
    message(STATUS "Found Gperftools")
    include_directories(${GPERFTOOLS_INCLUDE_DIR})

    # set (FLAGS "-Wl,--no-as-needed -l${GPERFTOOLS_TCMALLOC_AND_PROFILER} -Wl,--as-needed -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
    # set_property (TARGET firestorm APPEND PROPERTY LINK_FLAGS "${FLAGS}")
    target_link_libraries(firestorm ${GPERFTOOLS_LIBRARIES})

    add_definitions(-DWITH_GPERFTOOLS)
endif()

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(firestorm PUBLIC cxx_constexpr)

# Print a summary of the selected options.
include(FeatureSummary)
feature_summary(WHAT ALL)
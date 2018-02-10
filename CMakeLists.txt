cmake_minimum_required(VERSION 3.6)
project(firestorm)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/eigen/cmake/;${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCE_FILES main.cpp include/Simd.h include/mem_chunk_t.h include/ChunkManager.h include/bytes_t.h include/context_t.h include/index_t.h include/vector_insert_t.h include/Worker.h include/chunk_idx_t.h include/ChunkVisitor.h include/ChunkAccessor.h include/DotProductVisitorAvx.h include/NaiveUnrolledVisitorAvx.h include/result_t.h include/dot_product_avx256.h dot_product_avx256.cpp include/dot_product_naive.h)
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
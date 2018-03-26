# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(libfirestorm-benchmark VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/benchmark.cpp src/query_vector.cpp src/test_round.cpp src/test_round_worker.cpp src/test_round_worker_mt.cpp src/vector_generator.cpp)

set(HEADER_FILES
        include/firestorm/benchmark/benchmark.h src/query_vector.h src/test_round.h include/firestorm/benchmark/vector_generator.h)

# Define an executable
add_library(libfirestorm-benchmark
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(libfirestorm-benchmark PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(libfirestorm-benchmark
        boost
        libfirestorm
        libfirestorm-utils
        ${CMAKE_THREAD_LIBS_INIT})

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(libfirestorm-benchmark
        PUBLIC cxx_constexpr cxx_auto_type cxx_final cxx_override cxx_nullptr cxx_user_literals
        PRIVATE cxx_alignas)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result AND FSTM_ENABLE_LTO)
    set_property(TARGET libfirestorm-benchmark PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

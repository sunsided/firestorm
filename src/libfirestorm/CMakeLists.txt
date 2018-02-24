# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(libfirestorm VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/dot_product_avx256.cpp
        src/dot_product_naive.cpp
        src/dot_product_openmp.cpp
        src/dot_product_sse42.cpp)

set(HEADER_FILES
        include/firestorm/bytes_t.h
        include/firestorm/chunk_idx_t.h
        include/firestorm/ChunkAccessor.h
        include/firestorm/ChunkManager.h
        include/firestorm/ChunkVisitor.h
        include/firestorm/context_t.h
        include/firestorm/dot_product_openmp.h
        include/firestorm/dot_product_avx256.h
        include/firestorm/dot_product_naive.h
        include/firestorm/dot_product_functor.h
        include/firestorm/dot_product_sse42.h
        include/firestorm/DotProductVisitor.h
        include/firestorm/index_t.h
        include/firestorm/mem_chunk_t.h
        include/firestorm/result_t.h
        include/firestorm/Simd.h
        include/firestorm/vector_idx_t.h
        include/firestorm/vector_insert_t.h
        include/firestorm/vector_t.h
        include/firestorm/Worker.h)

# Define an executable
add_library(libfirestorm
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(libfirestorm PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(libfirestorm
        boost
        gperftools)

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(libfirestorm
        PUBLIC cxx_constexpr cxx_auto_type cxx_final cxx_override cxx_nullptr cxx_user_literals
        PRIVATE cxx_alignas)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result)
    set_property(TARGET libfirestorm PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

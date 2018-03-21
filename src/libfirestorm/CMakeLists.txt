# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(libfirestorm VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/vector_ops/dot_product_avx256.cpp
        src/vector_ops/dot_product_naive.cpp
        src/vector_ops/dot_product_openmp.cpp
        src/vector_ops/dot_product_sse42.cpp
        src/worker/worker_thread_coordinator.cpp
        src/thread_support.cpp
        src/combiner/reducer_factory.cpp src/job/job_coordinator.cpp src/job/job_info_t.cpp src/job/job_tracker.h)

set(HEADER_FILES
        src/worker/worker_cmd_t.h
        src/worker/worker_thread.h
        include/firestorm/engine/types/bytes_t.h
        include/firestorm/engine/types/chunk_idx_t.h
        include/firestorm/engine/memory/ChunkAccessor.h
        include/firestorm/engine/memory/ChunkManager.h
        include/firestorm/engine/types/context_t.h
        include/firestorm/engine/vector_ops/dot_product_openmp.h
        include/firestorm/engine/vector_ops/dot_product_avx256.h
        include/firestorm/engine/vector_ops/dot_product_naive.h
        include/firestorm/engine/vector_ops/dot_product_functor.h
        include/firestorm/engine/vector_ops/dot_product_sse42.h
        include/firestorm/engine/mapper/dot_product_mapper.h
        include/firestorm/engine/types/index_t.h
        include/firestorm/engine/types/mem_chunk_t.h
        include/firestorm/engine/types/result_t.h
        include/firestorm/engine/types/score_t.h
        include/firestorm/engine/simd.h
        include/firestorm/engine/types/vector_idx_t.h
        include/firestorm/engine/types/vector_insert_t.h
        include/firestorm/engine/types/vector_t.h
        include/firestorm/engine/worker/worker_t.h
        include/firestorm/engine/worker/worker_thread_coordinator.h
        include/firestorm/engine/mapper/mapper_t.h
        include/firestorm/engine/mapper/mapper_factory.h
        include/firestorm/engine/mapper/dot_product_mapper.h
        include/firestorm/engine/mapper/dot_product_mapper_factory.h
        include/firestorm/engine/reducer/reducer_t.h
        include/firestorm/engine/reducer/reducer_factory.h
        include/firestorm/engine/reducer/keep_all_reducer.h
        include/firestorm/engine/reducer/keep_all_reducer_factory.h
        include/firestorm/engine/thread_support.h
        include/firestorm/engine/types/score_value_t.h
        include/firestorm/engine/job/job_t.h
        include/firestorm/engine/job/job_info_t.h
        include/firestorm/engine/mapper/map_result_t.h
        include/firestorm/engine/reducer/reduce_result_t.h include/firestorm/engine/job/job_coordinator.h include/firestorm/engine/job/job_status_t.h)

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
        gperftools
        concurrentqueue
        taocpp-operators
        libfirestorm-logging
        libfirestorm-utils
        ${CMAKE_THREAD_LIBS_INIT})

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(libfirestorm
        PUBLIC cxx_constexpr cxx_auto_type cxx_final cxx_override cxx_nullptr cxx_user_literals
        PRIVATE cxx_alignas)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result AND FSTM_ENABLE_LTO)
    set_property(TARGET libfirestorm PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

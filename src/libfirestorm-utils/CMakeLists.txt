# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(libfirestorm-utils VERSION 0.1 LANGUAGES C CXX)

# Disable clang diagnostic pragma unknown in GCC warning.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")

# Define library. Only source files here!
set(SOURCE_FILES
        src/exception.cpp
        src/reader_writer_lock.cpp
        src/auto_reset_event.cpp
        src/cancellation_token_source.cpp
        src/guid_generator.cpp
        src/logger_factory.cpp)

set(HEADER_FILES
        include/firestorm/utils/exception.h
        include/firestorm/utils/time_conversion.h
        include/firestorm/synchronization/reader_writer_lock.h
        include/firestorm/utils/future_support.h
        include/firestorm/synchronization/auto_reset_event.h
        include/firestorm/synchronization/completion_signal_t.h
        include/firestorm/synchronization/cancellation_token_source.h
        include/firestorm/synchronization/cancellation_exception.h
        include/firestorm/synchronization/cancellation_token.h
        include/firestorm/utils/guid_generator.h
        include/firestorm/utils/guid.h
        include/firestorm/logging/logger_t.h
        include/firestorm/logging/logger_factory.h
        include/firestorm/logging/logging_exception.h)

# Define an executable
add_library(libfirestorm-utils
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(libfirestorm-utils PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(libfirestorm-utils
        boost
        spdlog)

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(libfirestorm-utils
        PUBLIC cxx_noexcept cxx_deleted_functions cxx_defaulted_functions cxx_defaulted_move_initializers
        PRIVATE cxx_auto_type)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result AND FSTM_ENABLE_LTO)
    set_property(TARGET libfirestorm-utils PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

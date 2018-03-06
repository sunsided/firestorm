# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(firestorm VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/main.cpp
        src/options/option_verbosity.cpp)

set(HEADER_FILES
        src/options/options.h
        src/options/options_verbosity.h)

# Define an executable
add_executable(firestorm
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(firestorm
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            src
            src/options)

# Define the libraries this project depends upon
target_link_libraries(firestorm
        boost
        gperftools
        cli11
        libfirestorm
        libfirestorm-benchmark
        libfirestorm-logging)

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(firestorm
        PUBLIC cxx_constexpr cxx_auto_type cxx_nullptr cxx_range_for cxx_static_assert)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result AND FSTM_ENABLE_LTO)
    set_property(TARGET firestorm PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

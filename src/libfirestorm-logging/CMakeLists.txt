# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(libfirestorm-logging VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/LoggerFactory.cpp)

set(HEADER_FILES
        include/firestorm/logging/LoggerFactory.h include/firestorm/logging/LoggingException.h)

# Define an executable
add_library(libfirestorm-logging
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(libfirestorm-logging PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(libfirestorm-logging
        spdlog)

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(libfirestorm-logging
        PUBLIC cxx_noexcept cxx_deleted_functions
        PRIVATE cxx_auto_type cxx_defaulted_move_initializers cxx_defaulted_functions)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result AND FSTM_ENABLE_LTO)
    set_property(TARGET libfirestorm-logging PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

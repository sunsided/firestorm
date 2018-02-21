# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(liblogging VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/LoggerFactory.cpp)

set(HEADER_FILES
        include/logging/LoggerFactory.h include/logging/LoggingException.h)

# Define an executable
add_library(liblogging
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(liblogging PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(liblogging
        spdlog)

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(liblogging
        PRIVATE cxx_auto_type)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result)
    set_property(TARGET liblogging PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

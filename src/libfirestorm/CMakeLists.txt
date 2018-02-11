# TODO: https://rix0r.nl/blog/2015/08/13/cmake-guide/
project(libfirestorm VERSION 0.1 LANGUAGES C CXX)

# Define library. Only source files here!
set(SOURCE_FILES
        src/dot_product_avx256.cpp
        src/dot_product_naive.cpp)

# Define an executable
add_library(libfirestorm
        ${SOURCE_FILES})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(libfirestorm PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Define the libraries this project depends upon
target_link_libraries(libfirestorm
        boost gperftools)

# Required compiler features
# see https://cmake.org/cmake/help/v3.1/manual/cmake-compile-features.7.html
target_compile_features(libfirestorm
        PUBLIC cxx_constexpr cxx_auto_type)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result)
    set_property(TARGET libfirestorm PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

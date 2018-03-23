project(test_libfirestorm VERSION 0.1 LANGUAGES C CXX)

# Define executable. Only source files here!
set(SOURCE_FILES
        src/test_dot_product_naive.cpp
        src/test_dot_product_openmp.cpp
        src/test_dot_product_avx256.cpp
        src/VectorNorm.cpp src/DotProduct.cpp src/test_dot_product_sse42.cpp src/test_cancellation_token.cpp)

set(HEADER_FILES
        include/VectorNorm.h
        include/DotProduct.h)

# Define an executable
add_executable(test_libfirestorm
        ${SOURCE_FILES}
        ${HEADER_FILES})

# Include directories
target_include_directories(libfirestorm PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Add the tests
gtest_add_tests(test_libfirestorm "" AUTO)

# Define the libraries this project depends upon
target_link_libraries(test_libfirestorm
        libfirestorm
        libfirestorm-utils
        gtest
        gmock)

# Link-Time Optimization
check_ipo_supported(RESULT result)
if(result)
    set_property(TARGET test_libfirestorm PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

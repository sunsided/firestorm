cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_subdirectory(libfirestorm)
add_subdirectory(libfirestorm-benchmark)
add_subdirectory(libfirestorm-utils)
add_subdirectory(firestorm)

if(FSTM_BUILD_TESTS)
    add_subdirectory(test_libfirestorm)
endif()
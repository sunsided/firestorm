option(FSTM_BUILD_TESTS "Enable unit tests" ON)

if(FSTM_BUILD_TESTS)
    enable_testing()

    include(GoogleTest)

    include(ExternalProject)
    ExternalProject_Add(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            # GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/external/googletest"
            GIT_REMOTE_NAME origin
            GIT_TAG release-1.8.0
            GIT_SHALLOW On
            GIT_PROGRESS On
            SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/googletest"
            BINARY_DIR "${CMAKE_BINARY_DIR}/external/googletest/build"
            # CONFIGURE_COMMAND ""
            # BUILD_COMMAND ""
            INSTALL_COMMAND ""
            TEST_COMMAND ""
            CMAKE_ARGS
                -DCMAKE_BUILD_TYPE=RelWithDebInfo
                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/external/googletest/build/out/
                -Dgtest_force_shared_crt=ON
            LOG_BUILD ON
            )

    # Specify the include directories
    ExternalProject_Get_Property(googletest source_dir)
    set(gtest_INCLUDE_DIRS "${source_dir}/googletest/include")
    set(gmock_INCLUDE_DIRS "${source_dir}/googlemock/include")

    # Specify the library directories
    ExternalProject_Get_Property(googletest binary_dir)

    set(gtest_LIBGTEST "${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(gtest_LIBGTEST_MAIN "${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(gmock_LIBGMOCK "${CMAKE_FIND_LIBRARY_PREFIXES}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(gmock_LIBGMOCK_MAIN "${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}")

    set(gtest_LIBRARIES "${binary_dir}/out/${gtest_LIBGTEST}" "${binary_dir}/out/${gtest_LIBGTEST_MAIN}")
    set(gmock_LIBRARIES "${binary_dir}/out/${gmock_LIBGMOCK}")

    # Hack to fix 'Imported target "gtest" includes non-existent path' errors.
    file(MAKE_DIRECTORY ${gtest_INCLUDE_DIRS})
    file(MAKE_DIRECTORY ${gmock_INCLUDE_DIRS})

    add_library(gtest INTERFACE IMPORTED)
    set_target_properties(gtest PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${gtest_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${gtest_LIBRARIES}")
    add_dependencies(gtest googletest)

    add_library(gmock INTERFACE IMPORTED)
    set_target_properties(gmock PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${gmock_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${gmock_LIBRARIES}")
    add_dependencies(gmock googletest)
endif()

include(ExternalProject)
ExternalProject_Add(cli11_git
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11
        GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/external/cli11"
        GIT_REMOTE_NAME origin
        GIT_TAG v1.3.0
        GIT_SHALLOW On
        GIT_PROGRESS On
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/CLI11"
        BINARY_DIR "${CMAKE_BINARY_DIR}/external/CLI11/build"
        # CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/external/CLI11/build/out/
            -DCLI_SINGLE_FILE=ON
            -DCLI_EXAMPLES=OFF
            -DCLI_TESTING=OFF
        LOG_BUILD ON
        )

# Specify the include directories
ExternalProject_Get_Property(cli11_git source_dir)
set(cli11_INCLUDE_DIRS "${source_dir}/include")

# Hack to fix 'Imported target "gtest" includes non-existent path' errors.
file(MAKE_DIRECTORY ${cli11_INCLUDE_DIRS})

add_library(cli11 INTERFACE IMPORTED)
set_target_properties(cli11 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${cli11_INCLUDE_DIRS}")
add_dependencies(cli11 cli11_git)

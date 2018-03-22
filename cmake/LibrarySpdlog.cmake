include(ExternalProject)
ExternalProject_Add(gabime_spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/external/spdlog"
        GIT_REMOTE_NAME origin
        GIT_TAG v0.16.3
        GIT_SHALLOW On
        GIT_PROGRESS On
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/spdlog"
        BINARY_DIR "${CMAKE_BINARY_DIR}/external/spdlog/build"
        # CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/external/spdlog/build/out/
        LOG_BUILD ON
        )

# Specify the include directories
ExternalProject_Get_Property(gabime_spdlog source_dir)
set(spdlog_INCLUDE_DIRS "${source_dir}/include")

# Hack to fix 'Imported target "gtest" includes non-existent path' errors.
file(MAKE_DIRECTORY ${spdlog_INCLUDE_DIRS})

add_library(spdlog INTERFACE IMPORTED)
set_target_properties(spdlog PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${spdlog_INCLUDE_DIRS}")
add_dependencies(spdlog gabime_spdlog)

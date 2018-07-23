include(ExternalProject)
ExternalProject_Add(taocpp_operators_git
        GIT_REPOSITORY https://github.com/taocpp/operators.git
        GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/external/taocpp_operators"
        GIT_REMOTE_NAME origin
        GIT_TAG 1.0.0
        GIT_SHALLOW On
        GIT_PROGRESS On
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/taocpp_operators"
        BINARY_DIR "${CMAKE_BINARY_DIR}/external/taocpp_operators/build"
        # CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
	    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=<BINARY_DIR>/out/
        LOG_BUILD ON
        )

# Specify the include directories
ExternalProject_Get_Property(taocpp_operators_git source_dir)
set(taocpp_operators_INCLUDE_DIRS "${source_dir}/include")

# Hack to fix 'Imported target "taocpp_operators" includes non-existent path' errors.
file(MAKE_DIRECTORY ${taocpp_operators_INCLUDE_DIRS})

add_library(taocpp-operators INTERFACE IMPORTED)
set_target_properties(taocpp-operators PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${taocpp_operators_INCLUDE_DIRS}")
add_dependencies(taocpp-operators taocpp_operators_git)

include(ExternalProject)
ExternalProject_Add(concurrentqueue_git
        GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
        # GIT_SUBMODULES "${CMAKE_SOURCE_DIR}/external/concurrentqueue"
        GIT_REMOTE_NAME origin
        GIT_TAG v1.0.0-beta
        GIT_SHALLOW On
        GIT_PROGRESS On
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/concurrentqueue"
        BINARY_DIR "${CMAKE_BINARY_DIR}/external/concurrentqueue/build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )

# Specify the include directories
ExternalProject_Get_Property(concurrentqueue_git source_dir)
set(concurrentqueue_INCLUDE_DIRS "${source_dir}")

# Hack to fix 'Imported target "concurrentqueue" includes non-existent path' errors.
file(MAKE_DIRECTORY ${concurrentqueue_INCLUDE_DIRS})

add_library(concurrentqueue INTERFACE IMPORTED)
set_target_properties(concurrentqueue PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${concurrentqueue_INCLUDE_DIRS}")
add_dependencies(concurrentqueue concurrentqueue_git)

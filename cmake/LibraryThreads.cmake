find_package(Threads)

if (CMAKE_USE_PTHREADS_INIT)
    add_definitions(-DUSE_PTHREADS=1)
endif()

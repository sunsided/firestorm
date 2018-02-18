option(FSTM_WITH_PROFILER "Use profiler from Google Performance Tools" OFF)
option(FSTM_WITH_TCMALLOC "Use tcmalloc from Google Performance Tools" ON)

if(FSTM_WITH_PROFILER OR FSTM_WITH_TCMALLOC)
    set(FSTM_WITH_GPERFTOOLS ON)
    find_package(Gperftools 2.6)

    if(GPERFTOOLS_FOUND)
        add_library(gperftools INTERFACE IMPORTED)
        set_property(TARGET gperftools PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GPERFTOOLS_INCLUDE_DIR})

        # Profiler only
        if(FSTM_WITH_PROFILER AND (NOT FSTM_WITH_TCMALLOC))
            add_definitions(-DUSE_PROFILER)
            message(STATUS "Firestorm: Build with Gperftools profiler (tcmalloc disabled)")
            set_property(TARGET gperftools PROPERTY INTERFACE_LINK_LIBRARIES ${GPERFTOOLS_PROFILER})
        # tcmalloc only
        elseif((NOT FSTM_WITH_PROFILER) AND FSTM_WITH_TCMALLOC)
            message(STATUS "Firestorm: Build with Gperftools tcmalloc (profiler disabled)")
            set_property(TARGET gperftools PROPERTY INTERFACE_LINK_LIBRARIES ${GPERFTOOLS_TCMALLOC})
        # Profiler and tcmalloc
        else()
            add_definitions(-DUSE_PROFILER)
            message(STATUS "Firestorm: Build with Gperftools tcmalloc and profiler")
            set_property(TARGET gperftools PROPERTY INTERFACE_LINK_LIBRARIES ${GPERFTOOLS_TCMALLOC_AND_PROFILER})
        endif()
    endif()
endif()
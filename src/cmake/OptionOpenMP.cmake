OPTION (FSTM_USE_OpenMP "Use OpenMP" ON)
if (FSTM_USE_OpenMP)
    find_package(OpenMP 4.0)
    if(OpenMP_CXX_FOUND)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        add_definitions(-DOPENMP=1)
    endif()
    if(OpenMP_C_FOUND)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        add_definitions(-DOPENMP=1)
    endif()
endif()
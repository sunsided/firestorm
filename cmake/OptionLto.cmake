include(CheckIPOSupported)

option(FSTM_ENABLE_LTO "Enables link-time optimization when building." ON)

check_ipo_supported(RESULT result)
if(FSTM_ENABLE_LTO AND result)
    message(STATUS "Firestorm: Enabling link-time optimizations.")
elseif(NOT FSTM_ENABLE_LTO)
    message(STATUS "Firestorm: Link-time optimization disabled.")
elseif(NOT result)
    message(STATUS "Firestorm: Link-time optimization not available.")
endif()
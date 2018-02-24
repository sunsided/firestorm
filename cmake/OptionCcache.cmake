find_program(CCACHE_FOUND ccache)
option(FSTM_ENABLE_CCACHE "Enables ccache support when building." CCACHE_FOUND)

if(CCACHE_FOUND AND FSTM_ENABLE_CCACHE)
    message(STATUS "Firestorm: Enabling ccache builds")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()
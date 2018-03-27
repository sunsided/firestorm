# TODO: Allow specifying endianness manually in order to cross-compile
include(TestBigEndian)

test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    add_definitions(-DBIG_ENDIAN -DENDIANNESS=1)
else(IS_BIG_ENDIAN)
    add_definitions(-DLITTLEN_ENDIAN -DENDIANNESS=0)
endif(IS_BIG_ENDIAN)
#
# Find the native ZLIB includes and library
#


FIND_PATH(NATIVE_ZLIB_INCLUDE_PATH zlib.h
/usr/local/include
/usr/include
)

FIND_LIBRARY(NATIVE_ZLIB_LIB_PATH z
/usr/lib
/usr/local/lib
)

IF(NATIVE_ZLIB_INCLUDE_PATH)
  SET(NATIVE_ZLIB_LIBRARY "-lz" CACHE)
ENDIF(NATIVE_ZLIB_INCLUDE_PATH)


#
# Find the native PNG includes and library
#


FIND_PATH(NATIVE_PNG_INCLUDE_PATH png.h
/usr/local/include
/usr/include
)

FIND_LIBRARY(NATIVE_PNG_LIB_PATH png
/usr/lib
/usr/local/lib
)

IF(NATIVE_PNG_INCLUDE_PATH)
  SET(NATIVE_PNG_LIBRARY "-lpng" CACHE)
ENDIF(NATIVE_PNG_INCLUDE_PATH)


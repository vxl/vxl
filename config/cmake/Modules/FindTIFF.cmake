#
# Find the native TIFF includes and library
#


FIND_PATH(NATIVE_TIFF_INCLUDE_PATH tiff.h
/usr/local/include
/usr/include
)

FIND_LIBRARY(NATIVE_TIFF_LIB_PATH tiff
/usr/lib
/usr/local/lib
)

IF(NATIVE_TIFF_INCLUDE_PATH)
  SET(NATIVE_TIFF_LIBRARY "-ltiff" CACHE)
ENDIF(NATIVE_TIFF_INCLUDE_PATH)


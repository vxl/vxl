#
# Find the native TIFF includes and library
#

IF(NOT HAS_NATIVE_TIFF)


  FIND_PATH(NATIVE_TIFF_INCLUDE_PATH tiff.h
    /usr/local/include
    /usr/include
  )
  
  FIND_LIBRARY(NATIVE_TIFF_LIBRARIES tiff
    /usr/lib
    /usr/local/lib
  )
  
  IF(NATIVE_TIFF_INCLUDE_PATH)
  IF(NATIVE_TIFF_LIBRARIES)
  
    SET( HAS_NATIVE_TIFF "YES" )
  
  ENDIF(NATIVE_TIFF_LIBRARIES)
  ENDIF(NATIVE_TIFF_INCLUDE_PATH)
ENDIF(NOT HAS_NATIVE_TIFF)

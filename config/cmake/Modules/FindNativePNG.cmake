#
# Find the native PNG includes and library
#

IF(NOT USE_NATIVE_PNG)  
  FIND_PATH( NATIVE_PNG_INCLUDE_PATH png.h
    /usr/local/include
    /usr/include
  )
  
  FIND_LIBRARY( NATIVE_PNG_LIBRARY png
    /usr/lib
    /usr/local/lib
  )
  
  IF(NATIVE_PNG_INCLUDE_PATH)
  IF(NATIVE_PNG_LIBRARY)
  
    SET( USE_NATIVE_PNG "YES" CACHE BOOL "Is there a native PNG library? Turn off to use v3p/png instead." )
  
    INCLUDE_DIRECTORIES( ${NATIVE_PNG_INCLUDE_PATH} )
    LINK_LIBRARIES( ${NATIVE_PNG_LIBRARY} )
  
  ENDIF(NATIVE_PNG_LIBRARY)
  ENDIF(NATIVE_PNG_INCLUDE_PATH)
ENDIF(NOT USE_NATIVE_PNG)

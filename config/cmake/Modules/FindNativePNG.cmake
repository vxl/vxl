#
# Find the native PNG includes and library
#

IF(NOT HAS_NATIVE_PNG)


  # PNG depends on Zlib
  INCLUDE( ${MODULE_PATH}/FindZLIB.cmake )
  
  IF(HAS_ZLIB)
    FIND_PATH( NATIVE_PNG_BASE_INCLUDE_PATH png.h
      /usr/local/include
      /usr/include
    )

    FIND_LIBRARY( NATIVE_PNG_LIBRARY png
      /usr/local/lib
      /usr/lib
    )

    IF(NATIVE_PNG_INCLUDE_PATH)
    IF(NATIVE_PNG_LIBRARY)

      SET( HAS_NATIVE_PNG "YES" )

      # png.h includes zlib.h. Sigh.
      SET( NATIVE_PNG_INCLUDE_PATH ${NATIVE_PNG_BASE_INCLUDE_PATH} ${ZLIB_INCLUDE_PATH} )
      SET( NATIVE_PNG_LIBRARIES ${NATIVE_PNG_LIBRARY} ${ZLIB_LIBRARIES} )

    ENDIF(NATIVE_PNG_LIBRARY)
    ENDIF(NATIVE_PNG_INCLUDE_PATH)
  ENDIF(HAS_ZLIB)
ENDIF(NOT HAS_NATIVE_PNG)

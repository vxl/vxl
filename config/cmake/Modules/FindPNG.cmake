#
# Find a PNG library
#

IF(NOT HAS_PNG)


  INCLUDE( ${MODULE_PATH}/FindNativePNG.cmake )
  
  IF(NOT HAS_NATIVE_PNG)

    INCLUDE( ${MODULE_PATH}/FindZLIB.cmake ) 
    IF(HAS_ZLIB)

    #
    # At some point, in a "release" version, it is possible that someone
    # will not have the v3p png library, so make sure the headers
    # exist.
    #
  
    FIND_PATH( PNG_BASE_INCLUDE_PATH png.h
      ${allvxl_SOURCE_DIR}/v3p/png
    )

    IF(PNG_BASE_INCLUDE_PATH)
  
      SET( HAS_PNG "YES" )
      ADD_DEFINITIONS( -DHAS_PNG )
      SET( PNG_LIBRARIES png )
      # png.h includes zlib.h. Sigh.
      SET( PNG_INCLUDE_PATH ${PNG_BASE_INCLUDE_PATH} ${ZLIB_INCLUDE_PATH} )

      # See the comments in v3p/png/CMakeLists.txt
      # NB: make sure the logic here is consistent with the logic there!
      IF (CYGWIN)
        IF(BUILD_SHARED_LIBS)
           # No need to define PNG_USE_DLL here, because it's default for Cygwin.
        ELSE(BUILD_SHARED_LIBS)
          ADD_DEFINITIONS(-DPNG_STATIC)
        ENDIF(BUILD_SHARED_LIBS)
      ENDIF (CYGWIN)
  
    ENDIF(PNG_BASE_INCLUDE_PATH)
    ENDIF(HAS_ZLIB)
  
  ELSE(NOT HAS_NATIVE_PNG)
  
    SET( HAS_PNG "YES" )
    ADD_DEFINITIONS( -DHAS_PNG )
    SET( PNG_INCLUDE_PATH ${NATIVE_PNG_INCLUDE_PATH} )
    SET( PNG_LIBRARIES ${NATIVE_PNG_LIBRARIES} )
  
  ENDIF(NOT HAS_NATIVE_PNG)
ENDIF(NOT HAS_PNG)

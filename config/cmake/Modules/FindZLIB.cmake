#
# Find a ZLIB library
#

IF(NOT HAS_ZLIB)


  INCLUDE( ${MODULE_PATH}/FindNativeZLIB.cmake )
  
  IF(NOT HAS_NATIVE_ZLIB)
  
    #
    # At some point, in a "release" version, it is possible that someone
    # will not have the v3p zlib library, so make sure the headers
    # exist.
    #
  
    FIND_PATH( ZLIB_INCLUDE_PATH zlib.h
      ${allvxl_SOURCE_DIR}/v3p/zlib
    )
  
    IF(ZLIB_INCLUDE_PATH)
  
      SET( HAS_ZLIB "YES" )
      ADD_DEFINITIONS( -DHAS_ZLIB )

      SET(  ZLIB_LIBRARIES zlib )
  
    ENDIF(ZLIB_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_ZLIB)
  
    SET( HAS_ZLIB "YES" )
    ADD_DEFINITIONS( -DHAS_ZLIB )
    SET( ZLIB_INCLUDE_PATH ${NATIVE_ZLIB_INCLUDE_PATH} )
    SET( ZLIB_LIBRARIES ${NATIVE_ZLIB_LIBRARIES} )
  
  ENDIF(NOT HAS_NATIVE_ZLIB)
ENDIF(NOT HAS_ZLIB)

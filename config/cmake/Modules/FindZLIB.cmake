#
# Find a ZLIB library
#

IF(NOT HAS_ZLIB)
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeZLIB.cmake )
  
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
      INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_PATH} )

      INCLUDE( ${allvxl_SOURCE_DIR}/v3p/zlib/CMakeListsLink.txt )
  
    ENDIF(ZLIB_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_ZLIB)
  
    SET( HAS_ZLIB "YES" )
    ADD_DEFINITIONS( -DHAS_ZLIB )
  
  ENDIF(NOT HAS_NATIVE_ZLIB)
ENDIF(NOT HAS_ZLIB)

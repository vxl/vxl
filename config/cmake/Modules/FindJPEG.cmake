#
# Find a JPEG library
#

IF(NOT HAS_JPEG)
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeJPEG.cmake )
  
  IF(NOT HAS_NATIVE_JPEG)
  
    #
    # At some point, in a "release" version, it is possible that someone
    # will not have the v3p jpeg library
    #
  
    FIND_PATH( JPEG_INCLUDE_PATH jpeglib.h
      ${allvxl_SOURCE_DIR}/v3p/jpeg
    )
  
    IF(JPEG_INCLUDE_PATH)
  
      SET( HAS_JPEG "YES" )
      ADD_DEFINITIONS( -DHAS_JPEG )
      INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_PATH} )
      SET( JPEG_LIBRARIES jpeg )
  
      INCLUDE( ${allvxl_SOURCE_DIR}/v3p/jpeg/CMakeListsLink.txt )
  
    ENDIF(JPEG_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_JPEG)
  
    SET( HAS_JPEG "YES" )
    ADD_DEFINITIONS( -DHAS_JPEG )
  
  ENDIF(NOT HAS_NATIVE_JPEG)
ENDIF(NOT HAS_JPEG)
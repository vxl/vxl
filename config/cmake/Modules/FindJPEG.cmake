#
# Find a JPEG library
#

IF(NOT HAS_JPEG)

  INCLUDE( ${MODULE_PATH}/FindNativeJPEG.cmake )
  
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
  
    ENDIF(JPEG_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_JPEG)
  
    SET( JPEG_INCLUDE_PATH ${NATIVE_JPEG_INCLUDE_PATH} )
    SET( JPEG_LIBRARIES ${NATIVE_JPEG_LIBRARIES} )
    SET( HAS_JPEG "YES" )
    ADD_DEFINITIONS( -DHAS_JPEG )
  
  ENDIF(NOT HAS_NATIVE_JPEG)
ENDIF(NOT HAS_JPEG)

#
# Find a TIFF library
#

IF(NOT HAS_TIFF)


  INCLUDE( ${MODULE_PATH}/FindNativeTIFF.cmake )
  
  IF(NOT HAS_NATIVE_TIFF)
  
    #
    # At some point, in a "release" version, it is possible that someone
    # will not have the v3p tiff library, so make sure the headers
    # exist.
    #
  
    FIND_PATH( TIFF_INCLUDE_PATH tiff.h
      ${allvxl_SOURCE_DIR}/v3p/tiff
    )
  
    IF(TIFF_INCLUDE_PATH)
  
      SET( HAS_TIFF "YES" )
      ADD_DEFINITIONS( -DHAS_TIFF )
  
      SET( TIFF_LIBRARIES tiff )
  
    ENDIF(TIFF_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_TIFF)
  
    SET( HAS_TIFF "YES" )
    ADD_DEFINITIONS( -DHAS_TIFF )
    SET( TIFF_INCLUDE_PATH ${NATIVE_TIFF_INCLUDE_PATH} )
    SET( TIFF_LIBRARIES ${NATIVE_TIFF_LIBRARIES} )
  
  ENDIF(NOT HAS_NATIVE_TIFF)
ENDIF(NOT HAS_TIFF)

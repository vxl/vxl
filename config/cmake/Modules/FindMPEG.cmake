#
# Find a MPEG library
#

IF(NOT HAS_MPEG)


  INCLUDE( ${MODULE_PATH}/FindNativeMPEG.cmake )
  
  IF(NOT HAS_NATIVE_MPEG)

    # There is no mpeg library in v3p. If you don't have a native library installed,
    # then you don't have mpeg support.

  ELSE(NOT HAS_NATIVE_MPEG)

    SET( MPEG_INCLUDE_PATH ${NATIVE_MPEG_INCLUDE_PATH}/mpeg2dec/include )
    SET( MPEG_LIBRARIES ${NATIVE_MPEG_LIBRARIES} )
  
    SET( HAS_MPEG "YES" )
    ADD_DEFINITIONS( -DHAS_MPEG )
  
  ENDIF(NOT HAS_NATIVE_MPEG)

ENDIF(NOT HAS_MPEG)

#
# Find a MPEG library
#

IF(NOT HAS_MPEG)
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeMPEG.cmake )
  
  IF(NOT HAS_NATIVE_MPEG)

    # There is no mpeg library in v3p. If you don't have a native library installed,
    # then you don't have mpeg support - Amitha Perera.

    # #
    # # At some point, in a "release" version, it is possible that someone
    # # will not have the v3p png library, so make sure the headers
    # # exist.
    # #
    # 
    # FIND_PATH( MPEG_INCLUDE_PATH png.h
    #   ${allvxl_SOURCE_DIR}/v3p/png
    # )
    #
    # IF(MPEG_INCLUDE_PATH)
    #  
    #   SET( HAS_MPEG "YES" )
    #   ADD_DEFINITIONS( -DHAS_MPEG )
    # 
    #  INCLUDE( ${allvxl_SOURCE_DIR}/v3p/png/CMakeListsLink.txt )
    # 
    # ENDIF(MPEG_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_MPEG)
  
    SET( HAS_MPEG "YES" )
    ADD_DEFINITIONS( -DHAS_MPEG )
  
  ENDIF(NOT HAS_NATIVE_MPEG)

ENDIF(NOT HAS_MPEG)

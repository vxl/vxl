#
# Find a PNG library
#

IF(NOT HAS_PNG)
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativePNG.cmake )
  
  IF(NOT HAS_NATIVE_PNG)
  
    #
    # At some point, in a "release" version, it is possible that someone
    # will not have the v3p png library, so make sure the headers
    # exist.
    #
  
    FIND_PATH( PNG_INCLUDE_PATH png.h
      ${allvxl_SOURCE_DIR}/v3p/png
    )
  
    IF(PNG_INCLUDE_PATH)
  
      SET( HAS_PNG "YES" )
      ADD_DEFINITIONS( -DHAS_PNG )
  
      INCLUDE( ${allvxl_SOURCE_DIR}/v3p/png/CMakeListsLink.txt )
  
    ENDIF(PNG_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_PNG)
  
    SET( HAS_PNG "YES" )
    ADD_DEFINITIONS( -DHAS_PNG )
  
  ENDIF(NOT HAS_NATIVE_PNG)

  # libpng uses zlib for compressed PNG files
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindZLIB.cmake )
ENDIF(NOT HAS_PNG)

#
# Find a TIFF library
#
# This file is used to manage using either a natively provided TIFF library or the one in v3p if provided.
#
#
# As per the standard scheme the following definitions are used
# TIFF_INCLUDE_DIR - where to find tiff.h
# TIFF_LIBRARIES   - the set of libraries to include to use TIFF.
# TIFF_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_TIFF  - True if we are using a TIFF library provided outside vxl (or v3p)


INCLUDE( ${MODULE_PATH}/NewCMake/FindTIFF.cmake )
  
IF(NOT TIFF_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the v3p tiff library, so make sure the headers
  # exist.
  #
  
  IF(EXISTS ${allvxl_SOURCE_DIR}/v3p/tiff/tiff.h)

    SET( TIFF_FOUND "YES" )
    SET( TIFF_INCLUDE_DIR ${allvxl_SOURCE_DIR}/v3p/tiff)  
    SET( TIFF_LIBRARIES tiff )
  
  ENDIF(EXISTS ${allvxl_SOURCE_DIR}/v3p/tiff/tiff.h)
  
ENDIF(NOT TIFF_FOUND)
  

IF(TIFF_LIBRARY)
  SET(VXL_USING_NATIVE_TIFF "YES")
ENDIF(TIFF_LIBRARY)

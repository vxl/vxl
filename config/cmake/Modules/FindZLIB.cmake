#
# Find a ZLIB library
#
# This file is used to manage using either a natively provided ZLIB library or the one in v3p if provided.
#
#
# As per the standard scheme the following definitions are used
# ZLIB_INCLUDE_DIR - where to find zlib.h
# ZLIB_LIBRARIES   - the set of libraries to include to use ZLIB.
# ZLIB_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_ZLIB  - True if we are using a ZLIB library provided outside vxl (or v3p)


INCLUDE( ${MODULE_PATH}/NewCMake/FindZLIB.cmake )
  
IF(NOT ZLIB_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the v3p ZLIB library, so make sure the headers
  # exist.
  #
  
  IF(EXISTS ${allvxl_SOURCE_DIR}/v3p/zlib/zlib.h)

    SET( ZLIB_FOUND "YES" )
    SET( ZLIB_INCLUDE_DIR ${allvxl_SOURCE_DIR}/v3p/zlib)  
    SET( ZLIB_LIBRARIES zlib )
  
  ENDIF(EXISTS ${allvxl_SOURCE_DIR}/v3p/zlib/zlib.h)
ENDIF(NOT ZLIB_FOUND)
  
IF(ZLIB_LIBRARY)
  SET(VXL_USING_NATIVE_ZLIB "YES")
ENDIF(ZLIB_LIBRARY)

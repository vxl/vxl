#
# Find an MPEG2 library
#
# This file is used to manage using either a natively provided MPEG2
# library or the one in v3p if provided.
#
#
# As per the standard scheme the following definitions are used
# MPEG2_INCLUDE_DIR - where to find mpeg2.h
# MPEG2_LIBRARIES   - the set of libraries to include to use MPEG2.
# MPEG2_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_MPEG2  - True if we are using a MPEG2 library provided outside vxl (or v3p)

INCLUDE( ${MODULE_PATH}/NewCMake/FindMPEG2.cmake )

IF(NOT MPEG2_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the v3p mpeg2 library
  #

  IF(EXISTS ${allvxl_SOURCE_DIR}/v3p/mpeg2/include/mpeg2.h)

    SET( MPEG2_FOUND "YES" )
    SET( MPEG2_LIBRARIES mpeg2 vo )
    SET( MPEG2_INCLUDE_DIR ${allvxl_SOURCE_DIR}/v3p/mpeg2/include)

  ENDIF(EXISTS ${allvxl_SOURCE_DIR}/v3p/mpeg2/include/mpeg2.h)

ENDIF(NOT MPEG2_FOUND)

IF(MPEG2_LIBRARY)
  SET(VXL_USING_NATIVE_MPEG2 "YES")
ENDIF(MPEG2_LIBRARY)

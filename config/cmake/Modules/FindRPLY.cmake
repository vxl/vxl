#
# Find a RPLY library
#
#
# This file is used to manage using either a natively provided RPLY library or the one in v3p if provided.
#
#
# As per the standard scheme the following definitions are used
# RPLY_INCLUDE_DIR - where to find rplylib.h
# RPLY_LIBRARIES   - the set of libraries to include to use RPLY.
# RPLY_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_RPLY  - True if we are using a RPLY library provided outside vxl (or v3p)

# If this FORCE variable is unset or is FALSE, try to find a native library.
IF( VXL_FORCE_V3P_RPLY )
ELSE( VXL_FORCE_V3P_RPLY )
  INCLUDE( ${CMAKE_ROOT}/Modules/FindRPLY.cmake )
ENDIF( VXL_FORCE_V3P_RPLY )

IF(RPLY_FOUND)

  SET(VXL_USING_NATIVE_RPLY "YES")

ELSE(RPLY_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the v3p rply library
  #

  IF(EXISTS ${vxl_SOURCE_DIR}/v3p/rply/rply.h)

    SET( RPLY_FOUND "YES" )
    SET( RPLY_LIBRARIES rply )  
    SET( RPLY_INCLUDE_DIR ${vxl_SOURCE_DIR}/v3p/rply)
    SET( RPLY_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/v3p/rply)
        
  ENDIF(EXISTS ${vxl_SOURCE_DIR}/v3p/rply/rply.h)

ENDIF(RPLY_FOUND)

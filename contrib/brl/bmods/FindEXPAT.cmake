#
# Find an Expat library
#
#
# This file is used to manage using either a natively provided Expat library
# or the one in b3p if provided.
#
#
# As per the standard scheme the following definitions are used
# EXPAT_INCLUDE_DIR - where to find expat.h
# EXPAT_LIBRARIES   - the set of libraries to include to use Expat.
# EXPAT_FOUND       - TRUE, if available somewhere on the system.
# EXPATPP_INCLUDE_DIR - where to find expatpp.h
# EXPATPP_LIBRARIES   - the set of libraries to include to use ExpatPP.
# EXPATPP_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_EXPAT   - True if we are using an Expat library provided outside vxl (or b3p)
# VXL_USING_NATIVE_EXPATPP - True if we are using an ExpatPP library provided outside vxl (or b3p)

# If this FORCE variable is unset or is FALSE, try to find a native library.
IF( VXL_FORCE_B3P_EXPAT )
ELSE( VXL_FORCE_B3P_EXPAT )
  INCLUDE( ${CMAKE_ROOT}/Modules/FindEXPAT.cmake )
ENDIF( VXL_FORCE_B3P_EXPAT )

IF(EXPAT_FOUND)

  SET(VXL_USING_NATIVE_EXPAT "YES")

ELSE(EXPAT_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the b3p expat library
  #

  IF(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat/expat.h)
    SET( EXPAT_FOUND "YES" )
    SET( EXPAT_LIBRARIES expat )
    SET( EXPAT_LIBRARY expat )
    SET( EXPAT_INCLUDE_DIRS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat ${vxl_BINARY_DIR}/contrib/brl/b3p/expat )
    SET( EXPAT_INCLUDE_DIR ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat ${vxl_BINARY_DIR}/contrib/brl/b3p/expat )
    SET( EXPAT_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/b3p/expat)
  ENDIF(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat/expat.h)

ENDIF(EXPAT_FOUND)

MARK_AS_ADVANCED(EXPAT_INCLUDE_DIR EXPAT_LIBRARY)

IF(EXPAT_FOUND) # EXPATPP requires EXPAT

FIND_PATH(EXPATPP_INCLUDE_DIR NAMES expatpp.h)
FIND_LIBRARY(EXPATPP_LIBRARY NAMES expatpp libexpatpp)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EXPATPP DEFAULT_MSG EXPATPP_LIBRARY EXPATPP_INCLUDE_DIR)
IF(EXPATPP_FOUND)
  SET(EXPATPP_LIBRARIES ${EXPATPP_LIBRARY})
  SET(EXPATPP_INCLUDE_DIRS ${EXPATPP_INCLUDE_DIR})
  SET(VXL_USING_NATIVE_EXPATPP "YES")
ELSE(EXPATPP_FOUND)
  IF(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expatpp/expatpp.h)
    SET( EXPATPP_FOUND "YES" )
    SET( EXPATPP_LIBRARIES expatpp )
    SET( EXPATPP_LIBRARY expatpp )
    SET( EXPATPP_INCLUDE_DIRS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expatpp ${vxl_BINARY_DIR}/contrib/brl/b3p/expatpp )
    SET( EXPATPP_INCLUDE_DIR ${vxl_SOURCE_DIR}/contrib/brl/b3p/expatpp ${vxl_BINARY_DIR}/contrib/brl/b3p/expatpp )
    SET( EXPATPP_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/b3p/expatpp)
  ELSE(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expatpp/expatpp.h)
    SET(EXPATPP_LIBRARIES)
    SET(EXPATPP_INCLUDE_DIRS)
  ENDIF(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expatpp/expatpp.h)
ENDIF(EXPATPP_FOUND)

ENDIF(EXPAT_FOUND)

MARK_AS_ADVANCED(EXPATPP_INCLUDE_DIR EXPATPP_LIBRARY)

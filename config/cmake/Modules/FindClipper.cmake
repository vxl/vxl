#
# Find a CLIPPER library
#
# This file is used to manage using either a natively provided CLIPPER library or the one in v3p if provided.
#
#
# As per the standard scheme the following definitions are used
# CLIPPER_INCLUDE_DIR - where to find clipper.hpp
# CLIPPER_LIBRARIES   - the set of libraries to include to use CLIPPER.
# CLIPPER_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_CLIPPER  - True if we are using a CLIPPER library provided outside vxl (or v3p)

IF( NOT CLIPPER_FOUND )

  # If this FORCE variable is unset or is FALSE, try to find a native library.
  IF( NOT VXL_FORCE_V3P_CLIPPER )
    # ./FindGEOTIFF.cmake does this instead...
    #INCLUDE( ${MODULE_PATH}/NewCMake/FindGEOTIFF.cmake )
    FIND_PACKAGE( CLIPPER QUIET )
    IF( CLIPPER_FOUND )
      SET(VXL_USING_NATIVE_CLIPPER "YES")
    ENDIF( CLIPPER_FOUND )
  ENDIF( NOT VXL_FORCE_V3P_CLIPPER )

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the v3p clipper library, so make sure the headers
  # exist.
  #

  IF( NOT CLIPPER_FOUND )
    IF(EXISTS ${vxl_SOURCE_DIR}/v3p/clipper/clipper.hpp)

      SET( CLIPPER_FOUND "YES" )
      SET( CLIPPER_INCLUDE_DIR ${clipper_BINARY_DIR} ${clipper_SOURCE_DIR})
      SET( CLIPPER_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_DIR}/include/vxl/v3p/clipper)
      SET( CLIPPER_LIBRARIES clipper )

    ENDIF(EXISTS ${vxl_SOURCE_DIR}/v3p/clipper/clipper.hpp)
  ENDIF( NOT CLIPPER_FOUND )

ENDIF(NOT CLIPPER_FOUND)

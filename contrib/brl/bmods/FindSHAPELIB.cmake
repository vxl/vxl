#
# Find an ShapeLib (shape files) library
#
#
# This file is used to manage using either a natively provided shapelib library
# or the one in b3p if provided.
#
#
# As per the standard scheme the following definitions are used
# SHAPELIB_INCLUDE_DIR - where to find expat.h
# SHAPELIB_LIBRARIES   - the set of libraries to include to use shapelib.
# SHAPELIB_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_SHAPELIB  - True if we are using an shapelib/shapefil.h library provided outside vxl (or b3p)

# If this FORCE variable is unset or is FALSE, try to find a native library.
if( VXL_FORCE_B3P_SHAPELIB )
  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the b3p SHAPELIB library
  #

  if(EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/shapelib/shapefil.h)

    set( SHAPELIB_FOUND "YES" )
    set( SHAPELIB_LIBRARIES shapelib )
    set( SHAPELIB_INCLUDE_DIR ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/shapelib)
    set( SHAPELIB_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/b3p/shapelib)

  endif()

endif()

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
if(NOT EXPAT_FOUND)

  # If this FORCE variable is unset or is FALSE, try to find a native library.
  if( NOT VXL_FORCE_B3P_EXPAT )
    find_package(EXPAT)
    if(EXPAT_FOUND)
      set(VXL_USING_NATIVE_EXPAT "YES")
    endif()
  endif()


  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the b3p expat library, so make sure the headers
  # exist.
  #
  if(NOT EXPAT_FOUND)
    if(EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/expat/expat.h)
      set( EXPAT_FOUND "YES" )
      set( EXPAT_LIBRARIES expat )
      set( EXPAT_LIBRARY expat )
      set( EXPAT_INCLUDE_DIRS ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/expat ${BRL_BINARY_DIR}/b3p/expat )
      set( EXPAT_INCLUDE_DIR  ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/expat ${BRL_BINARY_DIR}/b3p/expat )
      set( EXPAT_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/b3p/expat)
    endif()
  endif()

endif()
#mark_as_advanced(EXPAT_INCLUDE_DIR EXPAT_LIBRARY)

if(EXPAT_FOUND) # EXPATPP requires EXPAT
  find_path(EXPATPP_SYSTEM_INCLUDE_DIR NAMES expatpp.h )
  find_library(EXPATPP_SYSTEM_LIBRARY NAMES expatpp libexpatpp )

  if( EXPATPP_SYSTEM_INCLUDE_DIR AND EXPATPP_SYSTEM_LIBRARY )
    set(VXL_USING_NATIVE_EXPATPP "YES")
    set(EXPATPP_LIBRARIES ${EXPATPP_SYSTEM_LIBRARY})
    set(EXPATPP_INCLUDE_DIRS ${EXPATPP_SYSTEM_INCLUDE_DIR})
  else()
    set(VXL_USING_NATIVE_EXPATPP NO)
    if(EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/expatpp/expatpp.h)
      set( EXPATPP_FOUND YES )
      set( EXPATPP_LIBRARIES expatpp )
      set( EXPATPP_LIBRARY expatpp )
      set( EXPATPP_INCLUDE_DIRS ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/expatpp ${BRL_BINARY_DIR}/b3p/expatpp )
      set( EXPATPP_INCLUDE_DIR ${VXL_ROOT_SOURCE_DIR}/contrib/brl/b3p/expatpp ${BRL_BINARY_DIR}/b3p/expatpp )
      set( EXPATPP_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/b3p/expatpp)
    else()
      unset(EXPATPP_SYSTEM_INCLUDE_DIR) #If not found, keep out of cmake window
      unset(EXPATPP_SYSTEM_LIBRARY)  # If not found, keep out of cmake window
      unset(EXPATPP_LIBRARIES)
      unset(EXPATPP_INCLUDE_DIRS)
      unset(EXPAT_FOUND)
      unset(EXPATPP_FOUND)
    endif()
  endif()

  include(FindPackageHandleStandardArgs)
  # FOUND_VAR EXPATPP_FOUND  not supported in cmake 2.8.9
  FIND_PACKAGE_HANDLE_STANDARD_ARGS( EXPATPP
             REQUIRED_VARS EXPATPP_LIBRARY EXPATPP_INCLUDE_DIR
             FAIL_MESSAGE "MISSING: "
     )
endif()
mark_as_advanced(EXPATPP_SYSTEM_INCLUDE_DIR EXPATPP_SYSTEM_LIBRARY)
mark_as_advanced(EXPATPP_INCLUDE_DIR EXPATPP_LIBRARY)

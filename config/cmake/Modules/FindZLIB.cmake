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
# VXL_USING_NATIVE_ZLIB  - True if we are using a ZLIB library provided outside v3p


# If this FORCE variable is unset or is FALSE, try to find a native library.
IF (NOT VXL_FORCE_V3P_ZLIB)
  INCLUDE( ${CMAKE_ROOT}/Modules/FindZLIB.cmake )
ENDIF (NOT VXL_FORCE_V3P_ZLIB)
  
IF (ZLIB_FOUND)

  SET(VXL_USING_NATIVE_ZLIB "YES")
  # All the other variables are set by CMake's FindZLIB. Don't
  # set them here.

ELSE (ZLIB_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the v3p ZLIB library, so make sure the headers exist.
  #
  
  IF (EXISTS ${vxl_SOURCE_DIR}/v3p/zlib/zlib.h)

    SET(ZLIB_INCLUDE_DIR ${vxl_SOURCE_DIR}/v3p/zlib CACHE PATH "What is the path where the file zlib.h can be found" FORCE)
    FIND_LIBRARY( ZLIB_LIBRARY z ${LIBRARY_OUTPUT_PATH} ${vxl_BINARY_DIR}/v3p/zlib )
    IF (ZLIB_LIBRARY)
      SET(ZLIB_LIBRARY ${ZLIB_LIBRARY} CACHE FILEPATH "Where can the z library be found" FORCE)
      SET(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
      SET(ZLIB_FOUND "YES")
    ENDIF (ZLIB_LIBRARY)
  
  ENDIF (EXISTS ${vxl_SOURCE_DIR}/v3p/zlib/zlib.h)
ENDIF (ZLIB_FOUND)

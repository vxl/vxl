#
# Root vxl
#
#   geoff.cross@crd.ge.com

# VXL can handle this statement, which introduces warnings about deprecations.
# However, it is commented out for the benefit of those building other projects
# within the VXL tree, but not fully conforming to CMake 1.4 standards.
# After a waiting period, this line should be uncommented.
# CMAKE_MINIMUM_REQUIRED(VERSION 1.4)

PROJECT(vxl)

SET( LIBRARY_OUTPUT_PATH ${vxl_BINARY_DIR}/lib CACHE PATH
     "Output directory for the vxl libraries" )

# This prevents UseVXL.cmake from attempting to load the main vxl
# cache.
SET( BUILDING_IN_VXL_TREE "YES" )

INCLUDE( ${vxl_SOURCE_DIR}/UseVXL.cmake )

# Copy the UseVXL.cmake file to the binary directory so that client
# projects don't need to find the source directory first. They can run
# the UseVXL.cmake from the vxl binary directory, and determine the
# vxl source directory by loading the cache.
CONFIGURE_FILE( ${vxl_SOURCE_DIR}/UseVXL.cmake
                ${vxl_BINARY_DIR}/UseVXL.cmake COPYONLY )

# To avoid typing for the FindXXX.cmake modules
SET( MODULE_PATH ${vxl_SOURCE_DIR}/config/cmake/Modules CACHE STATIC "VXL module path" )

# For use in client projects that call UseVXL.cmake
SET (VXL_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH} CACHE STATIC "Where all the vxl libraries are, for clients to use." )

# Build the core vxl + support libraries
SUBDIRS(vcl v3p core)

# Optionally build the contributed libraries
IF( EXISTS ${vxl_SOURCE_DIR}/contrib/CMakeLists.txt )
  OPTION( BUILD_CONTRIB "Build the contributed libraries?" YES )
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/CMakeLists.txt )

IF( BUILD_CONTRIB )
  SUBDIRS( contrib )
ENDIF( BUILD_CONTRIB )



IF(RUN_CONFIGURE)
  EXEC_PROGRAM( ${vxl_SOURCE_DIR}/configure
                ${vxl_BINARY_DIR} )
ENDIF(RUN_CONFIGURE)


# Some types of path names can cause havoc with regular expressions,
# so avoid those.
IF( ${vxl_BINARY_DIR} MATCHES  \\+ )
  MESSAGE(SEND_ERROR "You cannot have a + in your binary path")
ENDIF( ${vxl_BINARY_DIR} MATCHES  \\+ )
IF( ${vxl_SOURCE_DIR} MATCHES  \\+ )
  MESSAGE(SEND_ERROR "You cannot have a + in your source path")
ENDIF( ${vxl_SOURCE_DIR} MATCHES  \\+ )



# include CMakeListsLocal.txt from source directory if it exists
# also include it from the binary dir if different from source dir
IF( ${vxl_BINARY_DIR} MATCHES ${vxl_SOURCE_DIR} )
  INCLUDE( ${vxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
ELSE( ${vxl_BINARY_DIR} MATCHES ${vxl_SOURCE_DIR} )
  INCLUDE( ${vxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
  INCLUDE( ${vxl_BINARY_DIR}/CMakeListsLocal.txt OPTIONAL )
ENDIF( ${vxl_BINARY_DIR} MATCHES ${vxl_SOURCE_DIR} )

#
# Root vxl
#
#   geoff.cross@crd.ge.com

# VXL can handle this statement, which introduces warnings about deprecations.
# However, it is commented out for the benefit of those building other projects
# within the VXL tree, but not fully conforming to CMake 1.4 standards.
# After a waiting period, this line should be uncommented.
# CMAKE_MINIMUM_REQUIRED(VERSION 1.4)

PROJECT(allvxl)

SET( LIBRARY_OUTPUT_PATH ${allvxl_BINARY_DIR}/lib CACHE PATH
     "Output directory for the vxl libraries" )

# This prevents UseVXL.cmake from attempting to load the main vxl
# cache.
SET( BUILDING_IN_VXL_TREE "YES" )

INCLUDE( ${allvxl_SOURCE_DIR}/UseVXL.cmake )

# Copy the UseVXL.cmake file to the binary directory so that client
# projects don't need to find the source directory first. They can run
# the UseVXL.cmake from the vxl binary directory, and determine the
# vxl source directory by loading the cache.
CONFIGURE_FILE( ${allvxl_SOURCE_DIR}/UseVXL.cmake
                ${allvxl_BINARY_DIR}/UseVXL.cmake COPYONLY )

# To avoid typing for the FindXXX.cmake modules
SET( MODULE_PATH ${allvxl_SOURCE_DIR}/config.cmake/Modules CACHE STATIC "VXL module path" )

# For use in client projects that call UseVXL.cmake
SET (VXL_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH} CACHE STATIC "Where all the vxl libraries are, for clients to use." )


OPTION(BUILD_MUL "Should the Manchester University Library be built?" YES)
OPTION(BUILD_OXL "Should the Oxford University Library be built?" YES)
OPTION(BUILD_GEL "Should the GE Library be built?" YES)
OPTION(BUILD_TBL "Should the TB Library be built?" YES)
OPTION(BUILD_RPL "Should the RPI Library be built?" YES)
OPTION(BUILD_OUL "Should the Otago University package be built?" YES)
OPTION(BUILD_CONVERSIONS "Should the Conversions Examples be built?" YES)


SUBDIRS(vcl v3p vxl)

IF(BUILD_MUL)
  SUBDIRS(mul)
ENDIF(BUILD_MUL)

IF(BUILD_OXL)
  SUBDIRS(oxl)
ENDIF(BUILD_OXL)

IF(BUILD_GEL)
  SUBDIRS(gel)
ENDIF(BUILD_GEL)

IF(BUILD_TBL)
  SUBDIRS(tbl)
ENDIF(BUILD_TBL)

IF(BUILD_RPL)
  SUBDIRS(rpl)
ENDIF(BUILD_RPL)

IF(BUILD_OUL)
  SUBDIRS(oul)
ENDIF(BUILD_OUL)

IF(BUILD_CONVERSIONS)
  SUBDIRS(conversions)
ENDIF(BUILD_CONVERSIONS)

IF(RUN_CONFIGURE)
  EXEC_PROGRAM( ${allvxl_SOURCE_DIR}/configure
                ${allvxl_BINARY_DIR} )
ENDIF(RUN_CONFIGURE)

# include CMakeListsLocal.txt from source directory if it exists
# also include it from the binary dir if different from source dir
IF( ${allvxl_BINARY_DIR} MATCHES ${allvxl_SOURCE_DIR} )
  INCLUDE( ${allvxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
ELSE( ${allvxl_BINARY_DIR} MATCHES ${allvxl_SOURCE_DIR} )
  INCLUDE( ${allvxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
  INCLUDE( ${allvxl_BINARY_DIR}/CMakeListsLocal.txt OPTIONAL )
ENDIF( ${allvxl_BINARY_DIR} MATCHES ${allvxl_SOURCE_DIR} )

#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SET(BUILD_TESTS "NO" CACHE BOOL "Should the tests be build?")

SUBDIRS(vcl v3p vxl oxl gel mul)

# This will cause "deep" dependencies to be generated, but that is
# good. Generate dependencies only for vxl stuff.
#
INCLUDE_REGULAR_EXPRESSION( "^([vrogm](.|..|...|....)l/.+)|vcl_.+$" )


# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

BUILD_SHARED_LIBRARIES()

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)

IF(RUN_CONFIGURE)
  EXEC_PROGRAM( ${allvxl_SOURCE_DIR}/config.cmake/configure
                ${allvxl_BINARY_DIR} )
ENDIF(RUN_CONFIGURE)

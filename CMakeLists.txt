#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SET(BUILD_TESTS "NO")

SUBDIRS(vcl v3p vxl mul oxl tbl gel conversions)

# This will cause "deep" dependencies to be generated, but that is
# good. Generate dependencies only for vxl stuff.
#
INCLUDE_REGULAR_EXPRESSION( "^([vrogm](.|..|...|....)l/.+)|vcl_.+$" )


# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)


#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SET(BUILD_TESTS "NO")

SUBDIRS(vcl v3p vxl mul oxl tbl gel)

#  The CMakeLists.txts in conversions are WRONG:
#    a) USE_NETLIB := 1 has no meaning in CMake
#    b) ADD_EXECUTABLE is being used incorrectly
#    c) $IUEROOT is not defined in CMake
#
#  until the CMakeLists.txt have been written properly, this is not
#   part of the CMake build structure.
#
#  further: as Target is not build using CMake (and is not likely to
#   ever be built, I don't see the point in having CMake try and
#   link against something it doesn't know how to build: it will never
#   ever ever work)
# SUBDIRS( conversions )


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


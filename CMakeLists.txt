#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

OPTION(BUILD_TESTS "Should the tests be built?" NO)
OPTION(BUILD_SHARED_LIBS "Should shared libraries be the default?" NO)

SUBDIRS(vcl v3p vxl oxl gel mul)

# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)

IF(RUN_CONFIGURE)
  EXEC_PROGRAM( ${allvxl_SOURCE_DIR}/config.cmake/configure
                ${allvxl_BINARY_DIR} )
ENDIF(RUN_CONFIGURE)

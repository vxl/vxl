#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SUBDIRS(vcl v3p vxl oxl gel)

# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)


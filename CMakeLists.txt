#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SUBDIRS(vxl vcl v3p oxl tbl gel)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)



#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SUBDIRS(vxl vcl v3p)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

WIN32_INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)



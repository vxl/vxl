
#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

SUBDIRS(vxl vcl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

WIN32_INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)

# this seems to be needed by vnl_math.cxx
WIN32_DEFINES( -DVCL_CAN_STATIC_CONST_INIT_FLOAT )

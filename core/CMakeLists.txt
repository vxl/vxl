#
# ./vxl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)

SUBDIRS(vul vsl vbl vpl vil vnl vgl vidl)
SUBDIRS( vcgl vcsl )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(test_allvxl)
ENDIF( BUILD_TESTING )

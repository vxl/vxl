#
# ./vxl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)

SUBDIRS(vsl vbl vul vpl vil vnl vgl vidl)
SUBDIRS( vcgl vcsl )

IF( BUILD_TESTS)
  SUBDIRS(test_allvxl examples)
ENDIF( BUILD_TESTS)

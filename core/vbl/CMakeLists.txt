#
# ./vxl/vbl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(vbl vbl_sources)

SOURCE_FILES( vbl_sources
vbl_ref_count
vbl_bool_ostream
vbl_sort
vbl_test
vbl_qsort
)

AUX_SOURCE_DIRECTORY(Templates vbl_sources)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)

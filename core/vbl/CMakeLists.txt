# vxl/vbl/CMakeLists.txt

ADD_LIBRARY(vbl vbl_sources)

SOURCE_FILES( vbl_sources
vbl_ref_count
vbl_bool_ostream
vbl_sort
vbl_test
vbl_qsort
vbl_shared_pointer
vbl_bit_array_3d
)

AUX_SOURCE_DIRECTORY(Templates vbl_sources)

SUBDIRS(io)
IF( BUILD_TESTS)
  SUBDIRS(tests examples)
ENDIF( BUILD_TESTS)

# ./vxl/vbl/CMakeLists.txt

ADD_LIBRARY(vbl vbl_sources)

SOURCE_FILES( vbl_sources
  vbl_ref_count.cxx
  vbl_bool_ostream.cxx
  vbl_sort.cxx
  vbl_test.cxx
  vbl_qsort.cxx
  vbl_shared_pointer.cxx
  vbl_bit_array_2d.cxx
  vbl_bit_array_3d.cxx
  vbl_triple.txx
  vbl_test.h
)

AUX_SOURCE_DIRECTORY(Templates vbl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

SUBDIRS(io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

# ./vxl/vbl/io/tests/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )

ADD_EXECUTABLE(test_vbl_io test_vbl_io_sources)
ADD_TEST(vbl_test_vbl_io test_vbl_io)

SOURCE_FILES(test_vbl_io_sources test_vbl_io.cxx)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/io/tests/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

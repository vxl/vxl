# ./vxl/vbl/io/CMakeLists.txt

ADD_LIBRARY(vbl_io vbl_io_sources)

AUX_SOURCE_DIRECTORY(Templates vbl_io_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/io/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

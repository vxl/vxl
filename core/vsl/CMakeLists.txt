# ./vxl/vsl/CMakeLists.txt

ADD_LIBRARY(vsl vsl_sources)

SOURCE_FILES(vsl_sources
  vsl_binary_io.cxx
  vsl_binary_loader_base.cxx
  vsl_indent.cxx
  vsl_test.cxx
)

AUX_SOURCE_DIRECTORY(Templates vsl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vsl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

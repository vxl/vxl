# ./vxl/vpl/CMakeLists.txt

ADD_LIBRARY(vpl vpl_sources)

SOURCE_FILES( vpl_sources
  vpl.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

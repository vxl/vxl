# ./vxl/vul/io/CMakeLists.txt

ADD_LIBRARY(vul_io vul_io_sources)

SOURCE_FILES(vul_io_sources
  vul_io_user_info.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vul/io/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

# ./vxl/vgl/io/CMakeLists.txt

ADD_LIBRARY(vgl_io vgl_io_sources)

SOURCE_FILES(vgl_io_sources
  vgl_io_polygon.cxx
)

AUX_SOURCE_DIRECTORY(Templates vgl_io_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vgl/io/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

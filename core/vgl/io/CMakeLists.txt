#
# ./mul/vgl/io/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(vgl_io vgl_io_sources)


SOURCE_FILES(vgl_io_sources
vgl_io_polygon
)

AUX_SOURCE_DIRECTORY(Templates vgl_io_sources)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)

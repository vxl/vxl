# ./vxl/vpl/CMakeLists.txt

ADD_LIBRARY(vpl vpl_sources)

SOURCE_FILES( vpl_sources
  vpl.h         vpl.cxx
  vpl_mutex.h
)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

# ./vxl/vpl/CMakeLists.txt

SET( vpl_sources
  vpl.h         vpl.cxx
  vpl_fdopen.h  vpl_fdopen.cxx
  vpl_fileno.h  vpl_fileno.cxx
  vpl_mutex.h
)

ADD_LIBRARY(vpl ${vpl_sources})
TARGET_LINK_LIBRARIES( vpl vcl )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

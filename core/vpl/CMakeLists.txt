# ./vxl/vpl/CMakeLists.txt

doxygen_add_library(core/vpl
  PACKAGE core
  DESCRIPTION "POSIX Compatability Library"
  )

SET( vpl_sources
  vpl.h         vpl.cxx
  vpl_fdopen.h  vpl_fdopen.cxx
  vpl_fileno.h  vpl_fileno.cxx
  vpl_mutex.h
)

ADD_LIBRARY(vpl ${vpl_sources})
IF(NOT WIN32)
  SET_TARGET_PROPERTIES(vpl PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(NOT WIN32)

TARGET_LINK_LIBRARIES( vpl vcl )


IF (NOT UNIX)
  TARGET_LINK_LIBRARIES( vpl ws2_32 vcl )
ENDIF (NOT UNIX)

INSTALL_TARGETS(/lib vpl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpl ${vpl_sources})

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)

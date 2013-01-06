# brl/bbas/baio/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(baio_sources
    baio.h
   )

IF(WIN32)
  SET(baio_sources ${baio_sources}
      baio_windows.cxx
     )
ELSEIF(CYGWIN)
  SET(baio_sources ${baio_sources}
      baio_windows.cxx
     )
ELSEIF(APPLE)
  SET(baio_sources ${baio_sources}
      baio_osx.cxx
     )
ELSE(WIN32)
  SET(baio_sources ${baio_sources}
      baio_unix.cxx
     )
ENDIF(WIN32)

ADD_LIBRARY(baio ${baio_sources})

IF(APPLE)
    TARGET_LINK_LIBRARIES(baio c)
ELSEIF(UNIX)
    TARGET_LINK_LIBRARIES(baio rt)
ENDIF(APPLE)

#install the .h .txx and libs
INSTALL_TARGETS(/lib baio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/baio/ ${baio_sources})

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

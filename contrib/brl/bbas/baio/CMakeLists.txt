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

IF(UNIX)
  TARGET_LINK_LIBRARIES(baio rt)
ELSE(UNIX)
  TARGET_LINK_LIBRARIES(baio)
ENDIF(UNIX)

#tests
SUBDIRS(tests)

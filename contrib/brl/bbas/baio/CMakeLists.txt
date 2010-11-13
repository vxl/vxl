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
ELSEIF(UNIX)
  SET(baio_sources ${baio_sources}
      baio_unix.cxx
     )
ENDIF(WIN32)

ADD_LIBRARY(baio ${baio_sources})

IF(WIN32)
  TARGET_LINK_LIBRARIES(baio)
ELSEIF(APPLE)
  TARGET_LINK_LIBRARIES(baio)
ELSEIF(UNIX)
  TARGET_LINK_LIBRARIES(baio rt)
ENDIF(WIN32)

#tests
SUBDIRS(tests)

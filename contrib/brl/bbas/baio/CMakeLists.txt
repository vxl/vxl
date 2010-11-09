# brl/bbas/baio/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

IF(WIN32)
  SET(baio_sources
      baio_windows.cxx       baio.h
     )
  ADD_LIBRARY(baio ${baio_sources})
  TARGET_LINK_LIBRARIES(baio)
  #tests
  SUBDIRS(tests)
ELSEIF(APPLE)
  SET(baio_sources
    baio_osx.cxx       baio.h
   )
  ADD_LIBRARY(baio ${baio_sources})
  TARGET_LINK_LIBRARIES(baio)
ELSEIF(UNIX)
  SET(baio_sources
      baio_unix.cxx       baio.h
     )
  ADD_LIBRARY(baio ${baio_sources})
  TARGET_LINK_LIBRARIES(baio rt)
  #tests
  SUBDIRS(tests)
ENDIF(WIN32)


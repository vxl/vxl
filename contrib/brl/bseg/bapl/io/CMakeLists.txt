#brl/bbas/bapl/io/CMakeLists.txt

SET( bapl_io_sources
     bapl_io_dsift.h      bapl_io_dsift.cxx
   )
AUX_SOURCE_DIRECTORY( Templates bapl_io_sources )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

ADD_LIBRARY( bapl_io ${bapl_io_sources} )
TARGET_LINK_LIBRARIES( bapl_io bapl vbl vbl_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

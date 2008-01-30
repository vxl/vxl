# brl/bpro/bprb/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
SET(bprb_sources
   bprb_parameters.cxx     bprb_parameters.h        bprb_parameters_sptr.h
   bprb_parameters.txx
   bprb_process.cxx        bprb_process.h           bprb_process_sptr.h
)


AUX_SOURCE_DIRECTORY(Templates bprb_sources)

ADD_LIBRARY(bprb ${bprb_sources})

TARGET_LINK_LIBRARIES(bprb brdb vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

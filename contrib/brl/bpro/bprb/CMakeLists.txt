# brl/bpro/bprb/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )

IF(HAS_BXML)
SET(bprb_sources
   bprb_parameters.cxx             bprb_parameters.h            bprb_parameters_sptr.h
   bprb_parameters.txx
   bprb_process.cxx                bprb_process.h               bprb_process_sptr.h
   bprb_process_manager.txx        bprb_process_manager.h
   bprb_batch_process_manager.cxx  bprb_batch_process_manager.h
   bprb_null_process.cxx           bprb_null_process.h
)

ADD_DEFINITIONS(-DHAS_BPRB)

AUX_SOURCE_DIRECTORY(Templates bprb_sources)

ADD_LIBRARY(bprb ${bprb_sources})

TARGET_LINK_LIBRARIES(bprb brdb bxml vbl vsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
ENDIF(HAS_BXML)
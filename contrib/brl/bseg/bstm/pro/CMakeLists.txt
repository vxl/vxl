INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(bstm_pro_sources
    bstm_register.h                              bstm_register.cxx
    bstm_processes.h
   )
AUX_SOURCE_DIRECTORY(processes bstm_pro_sources)
ADD_LIBRARY(bstm_pro  ${bstm_pro_sources})

TARGET_LINK_LIBRARIES(bstm_pro bprb boxm2_util bstm bstm_io bstm_util boxm2_basic boct brdb vpgl vil vgl vul vbl vcl)

IF( BUILD_TESTING )
  add_subdirectory(tests)
ENDIF( BUILD_TESTING )

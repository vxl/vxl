INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

SET(boxm2_cpp_pro_sources
    boxm2_cpp_register.h                              boxm2_cpp_register.cxx
    boxm2_cpp_processes.h
   )
AUX_SOURCE_DIRECTORY(processes boxm2_cpp_pro_sources)
ADD_LIBRARY(boxm2_cpp_pro  ${boxm2_cpp_pro_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp_pro boxm2_cpp_algo brad bbas_pro bbgm_pro bprb brdb vbl_io expatpp vpgl vil vgl_xio vgl vul vbl vsl vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(bstm_cpp_pro_sources
    bstm_cpp_register.h                              bstm_cpp_register.cxx
    bstm_cpp_processes.h
   )
AUX_SOURCE_DIRECTORY(processes bstm_cpp_pro_sources)
ADD_LIBRARY(bstm_cpp_pro  ${bstm_cpp_pro_sources})

TARGET_LINK_LIBRARIES(bstm_cpp_pro bstm_io bstm_cpp_algo boxm2_pro boxm2 brdb vpgl vil vgl vul vbl vcl)
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${MUL_INCLUDE_DIR} )
set(bstm_pro_sources
    bstm_register.h                              bstm_register.cxx
    bstm_processes.h
   )
aux_source_directory(processes bstm_pro_sources)
add_library(bstm_pro  ${bstm_pro_sources})

target_link_libraries(bstm_pro bprb boxm2_util bstm bstm_io bstm_util boxm2_basic boct brdb vpgl vil vgl vul vbl vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
